
/**
* \file opt_array_splitting.cpp
*
* If an array is always dereferenced with a constant index, then
* split it apart into its elements, making it more amenable to other
* optimization passes.
*
* This skips uniform/varying arrays, which would need careful
* handling due to their ir->location fields tying them to the GL API
* and other shader stages.
*/

#include "ShaderCompilerCommon.h"
#include "ir.h"
#include "ir_visitor.h"
#include "ir_rvalue_visitor.h"
#include "ir_print_visitor.h"
#include "glsl_types.h"
#include "IRDump.h"

static bool debug = false;

namespace opt_array_splitting
{
	class variable_entry : public exec_node
	{
	public:
		variable_entry(ir_variable *var)
		{
			this->var = var;
			this->whole_array_access = 0;
			this->declaration = false;
			this->components = NULL;
			this->mem_ctx = NULL;
			if (var->type->is_array())
				this->size = var->type->length;
			else if (var->type->is_patch())
			  this->size = var->type->patch_length;
			else
				this->size = var->type->matrix_columns;
		}

		ir_variable *var; /* The key: the variable's pointer. */
		unsigned size; /* array length or matrix columns */

		/** Number of times the variable is referenced, including assignments. */
		unsigned whole_array_access;

		bool declaration; /* If the variable had a decl in the instruction stream */

		ir_variable **components;

		/** ralloc_parent(this->var) -- the shader's talloc context. */
		void *mem_ctx;
	};

} /* namespace */
using namespace opt_array_splitting;

/**
* This class does a walk over the tree, coming up with the set of
* variables that could be split by looking to see if they are arrays
* that are only ever constant-index dereferenced.
*/
class ir_array_reference_visitor : public ir_hierarchical_visitor
{
public:
	ir_array_reference_visitor(void)
	{
		this->mem_ctx = ralloc_context(NULL);
		this->variable_list.make_empty();
	}

	~ir_array_reference_visitor(void)
	{
		ralloc_free(mem_ctx);
	}

	bool get_split_list(exec_list *instructions, bool linked);

	virtual ir_visitor_status visit(ir_variable *);
	virtual ir_visitor_status visit(ir_dereference_variable *);
	virtual ir_visitor_status visit_enter(ir_dereference_array *);

	variable_entry *get_variable_entry(ir_variable *var);

	/* List of variable_entry */
	exec_list variable_list;

	void *mem_ctx;
};

variable_entry* ir_array_reference_visitor::get_variable_entry(ir_variable *var)
{
	check(var);

	if (var->mode != ir_var_auto &&
		var->mode != ir_var_temporary)
		return NULL;

	if (!(var->type->is_array() || var->type->is_matrix() || var->type->is_patch()))
	{
		return NULL;
	}

	/* If the array hasn't been sized yet, we can't split it.  After
	* linking, this should be resolved.
	*/
	if (var->type->is_array() && var->type->length == 0)
		return NULL;

	if (var->type->is_patch() && var->type->patch_length == 0)
		return NULL;

	foreach_iter(exec_list_iterator, iter, this->variable_list)
	{
		variable_entry *entry = (variable_entry *)iter.get();
		if (entry->var == var)
		{
			return entry;
		}
	}

	if (debug)
	{
		printf("*** ADDING %d %s\n", var->id, var->name);
	}
	variable_entry *entry = new(mem_ctx)variable_entry(var);
	this->variable_list.push_tail(entry);
	return entry;
}


ir_visitor_status ir_array_reference_visitor::visit(ir_variable *ir)
{
	variable_entry *entry = this->get_variable_entry(ir);

	if (entry)
		entry->declaration = true;

	return visit_continue;
}

ir_visitor_status ir_array_reference_visitor::visit(ir_dereference_variable *ir)
{
	variable_entry *entry = this->get_variable_entry(ir->var);

	/* If we made it to here, then the dereference of this array didn't
	* have a constant index (see the visit_continue_with_parent
	* below), so we can't split the variable.
	*/
	if (entry)
		entry->whole_array_access++;

	return visit_continue;
}

ir_visitor_status ir_array_reference_visitor::visit_enter(ir_dereference_array *ir)
{
	ir_dereference_variable *deref = ir->array->as_dereference_variable();
	if (!deref)
		return visit_continue;

	variable_entry *entry = this->get_variable_entry(deref->var);

	if (entry && !ir->array_index->as_constant())
		entry->whole_array_access++;

	return visit_continue_with_parent;
}

bool ir_array_reference_visitor::get_split_list(exec_list *instructions, bool linked)
{
	visit_list_elements(this, instructions);

	/* If the shaders aren't linked yet, we can't mess with global
	* declarations, which need to be matched by name across shaders.
	*/
	if (!linked)
	{
		foreach_list(node, instructions)
		{
			ir_variable *var = ((ir_instruction *)node)->as_variable();
			if (var)
			{
				variable_entry *entry = get_variable_entry(var);
				if (entry)
				{
					entry->remove();
				}
			}
		}
	}

	/* Trim out variables we found that we can't split. */
	foreach_iter(exec_list_iterator, iter, variable_list)
	{
		variable_entry *entry = (variable_entry *)iter.get();

		if (debug)
		{
			printf("array %d %s@%p: decl %d, whole_access %d\n", entry->var->id,
				entry->var->name, (void *)entry->var, entry->declaration,
				entry->whole_array_access);
		}

		if (!entry->declaration || entry->whole_array_access)
		{
			entry->remove();
		}
	}

	return !variable_list.is_empty();
}

/** This is the class that does the actual work of splitting. */
class ir_array_splitting_visitor : public ir_rvalue_visitor
{
public:
	ir_array_splitting_visitor(exec_list *vars)
	{
		this->variable_list = vars;
	}

	virtual ~ir_array_splitting_visitor()
	{
	}

	virtual ir_visitor_status visit_leave(ir_assignment *);

	void split_deref(ir_dereference **deref);
	void handle_rvalue(ir_rvalue **rvalue);
	variable_entry *get_splitting_entry(ir_variable *var);

	exec_list *variable_list;
	void *mem_ctx;
};

variable_entry* ir_array_splitting_visitor::get_splitting_entry(ir_variable *var)
{
	check(var);

	foreach_iter(exec_list_iterator, iter, *this->variable_list)
	{
		variable_entry *entry = (variable_entry *)iter.get();
		if (entry->var == var)
		{
			return entry;
		}
	}

	return NULL;
}

void ir_array_splitting_visitor::split_deref(ir_dereference **deref)
{
	ir_dereference_array *deref_array = (*deref)->as_dereference_array();
	if (!deref_array)
		return;

	ir_dereference_variable *deref_var = deref_array->array->as_dereference_variable();
	if (!deref_var)
		return;
	ir_variable *var = deref_var->var;

	variable_entry *entry = get_splitting_entry(var);
	if (!entry)
		return;

	ir_constant *constant = deref_array->array_index->as_constant();
	check(constant);

	if (constant->value.i[0] >= 0 && constant->value.i[0] < (int)entry->size)
	{
		*deref = new(entry->mem_ctx)
			ir_dereference_variable(entry->components[constant->value.i[0]]);
	}
	else
	{
		/* There was a constant array access beyond the end of the
		* array.  This might have happened due to constant folding
		* after the initial parse.  This produces an undefined value,
		* but shouldn't crash.  Just give them an uninitialized
		* variable.
		*/
		ir_variable *temp = new(entry->mem_ctx) ir_variable(deref_array->type,
			"undef",
			ir_var_temporary);
		entry->components[0]->insert_before(temp);
		*deref = new(entry->mem_ctx) ir_dereference_variable(temp);
	}
}

void ir_array_splitting_visitor::handle_rvalue(ir_rvalue **rvalue)
{
	if (!*rvalue)
		return;

	ir_dereference *deref = (*rvalue)->as_dereference();

	if (!deref)
		return;

	split_deref(&deref);
	*rvalue = deref;
}

ir_visitor_status ir_array_splitting_visitor::visit_leave(ir_assignment *ir)
{
	/* The normal rvalue visitor skips the LHS of assignments, but we
	* need to process those just the same.
	*/
	ir_rvalue *lhs = ir->lhs;

	handle_rvalue(&lhs);
	ir->lhs = lhs->as_dereference();

	ir->lhs->accept(this);

	handle_rvalue(&ir->rhs);
	ir->rhs->accept(this);

	if (ir->condition)
	{
		handle_rvalue(&ir->condition);
		ir->condition->accept(this);
	}

	return visit_continue;
}

bool optimize_split_arrays(exec_list *instructions, bool linked)
{
	ir_array_reference_visitor refs;
	if (!refs.get_split_list(instructions, linked))
	{
		return false;
	}

	void *mem_ctx = ralloc_context(NULL);

	/* Replace the decls of the arrays to be split with their split
	* components.
	*/
	foreach_iter(exec_list_iterator, iter, refs.variable_list)
	{
		variable_entry *entry = (variable_entry *)iter.get();
		const struct glsl_type *type = entry->var->type;
		const struct glsl_type *subtype;

		if (type->is_matrix())
		{
			auto* Scalar = type->get_scalar_type();
			check(Scalar->is_numeric());
			subtype = glsl_type::get_instance(Scalar->base_type, type->vector_elements, 1);
		}
		else if (type->is_patch())
		{
			subtype = type->inner_type;
		}
		else
		{
			subtype = type->fields.array;
		}

		entry->mem_ctx = ralloc_parent(entry->var);

		entry->components = ralloc_array(mem_ctx,
			ir_variable *,
			entry->size);

		for (unsigned int i = 0; i < entry->size; i++)
		{
			const char *name = ralloc_asprintf(mem_ctx, "%s_%d",
				entry->var->name, i);

			entry->components[i] =
				new(entry->mem_ctx) ir_variable(subtype, name, ir_var_temporary);
			entry->var->insert_before(entry->components[i]);
		}

		entry->var->remove();
	}

	ir_array_splitting_visitor split(&refs.variable_list);
	visit_list_elements(&split, instructions);

	if (debug)
	{
		_mesa_print_ir(instructions, NULL);
	}

	ralloc_free(mem_ctx);

	return true;

}
