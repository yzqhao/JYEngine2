
/**
* \file opt_structure_splitting.cpp
*
* If a structure is only ever referenced by its components, then
* split those components out to individual variables so they can be
* handled normally by other optimization passes.
*
* This skips structures like uniforms, which need to be accessible as
* structures for their access by the GL.
*/

#include "ShaderCompilerCommon.h"
#include "ir.h"
#include "ir_visitor.h"
#include "ir_print_visitor.h"
#include "ir_rvalue_visitor.h"
#include "glsl_types.h"
#include "LanguageSpec.h"

static bool debug = false;

// XXX using variable_entry2 here to avoid collision (MSVC multiply-defined
// function) with the variable_entry class seen in ir_variable_refcount.h
// Perhaps we can use the one in ir_variable_refcount.h and make this class
// here go away?
class variable_entry2 : public exec_node
{
public:
	variable_entry2(ir_variable *var)
	{
		this->var = var;
		this->whole_structure_access = 0;
		this->declaration = false;
		this->components = NULL;
		this->mem_ctx = NULL;
	}

	ir_variable *var; /* The key: the variable's pointer. */

	/** Number of times the variable is referenced, including assignments. */
	unsigned whole_structure_access;

	bool declaration; /* If the variable had a decl in the instruction stream */

	ir_variable **components;

	/** ralloc_parent(this->var) -- the shader's ralloc context. */
	void *mem_ctx;
};


class ir_structure_reference_visitor : public ir_hierarchical_visitor
{
public:
	ir_structure_reference_visitor(void)
	{
		this->mem_ctx = ralloc_context(NULL);
		this->variable_list.make_empty();
	}

	~ir_structure_reference_visitor(void)
	{
		ralloc_free(mem_ctx);
	}

	virtual ir_visitor_status visit(ir_variable *);
	virtual ir_visitor_status visit(ir_dereference_variable *);
	virtual ir_visitor_status visit_enter(ir_dereference_record *);
	virtual ir_visitor_status visit_enter(ir_assignment *);
	virtual ir_visitor_status visit_enter(ir_function_signature *);

	variable_entry2 *get_variable_entry2(ir_variable *var);

	/* List of variable_entry */
	exec_list variable_list;

	void *mem_ctx;
};

variable_entry2 *
ir_structure_reference_visitor::get_variable_entry2(ir_variable *var)
{
	check(var);

	if (!var->type->is_record() || var->mode == ir_var_uniform)
		return NULL;

	foreach_iter(exec_list_iterator, iter, this->variable_list)
	{
		variable_entry2 *entry = (variable_entry2 *)iter.get();
		if (entry->var == var)
			return entry;
	}

	variable_entry2 *entry = new(mem_ctx)variable_entry2(var);
	this->variable_list.push_tail(entry);
	return entry;
}


ir_visitor_status
ir_structure_reference_visitor::visit(ir_variable *ir)
{
	variable_entry2 *entry = this->get_variable_entry2(ir);

	if (entry)
		entry->declaration = true;

	return visit_continue;
}

ir_visitor_status
ir_structure_reference_visitor::visit(ir_dereference_variable *ir)
{
	ir_variable *const var = ir->variable_referenced();
	variable_entry2 *entry = this->get_variable_entry2(var);

	if (entry)
		entry->whole_structure_access++;

	return visit_continue;
}

ir_visitor_status
ir_structure_reference_visitor::visit_enter(ir_dereference_record *ir)
{
	(void)ir;
	/* Don't descend into the ir_dereference_variable below. */
	return visit_continue_with_parent;
}

ir_visitor_status
ir_structure_reference_visitor::visit_enter(ir_assignment *ir)
{
	/* If there are no structure references yet, no need to bother with
	* processing the expression tree.
	*/
	if (this->variable_list.is_empty())
		return visit_continue_with_parent;

	if (ir->lhs->as_dereference_variable() &&
		ir->rhs->as_dereference_variable() &&
		!ir->condition)
	{
		/* We'll split copies of a structure to copies of components, so don't
		* descend to the ir_dereference_variables.
		*/
		return visit_continue_with_parent;
	}
	return visit_continue;
}

ir_visitor_status
ir_structure_reference_visitor::visit_enter(ir_function_signature *ir)
{
	/* We don't want to descend into the function parameters and
	* dead-code eliminate them, so just accept the body here.
	*/
	visit_list_elements(this, &ir->body);
	return visit_continue_with_parent;
}

class ir_structure_splitting_visitor : public ir_rvalue_visitor
{
public:
	ir_structure_splitting_visitor(exec_list *vars)
	{
		this->variable_list = vars;
	}

	virtual ~ir_structure_splitting_visitor()
	{
	}

	virtual ir_visitor_status visit_enter(ir_texture *);
	virtual ir_visitor_status visit_leave(ir_assignment *);

	void split_deref(ir_dereference **deref);
	void handle_rvalue(ir_rvalue **rvalue);
	variable_entry2 *get_splitting_entry(ir_variable *var);

	exec_list *variable_list;
	void *mem_ctx;
};

variable_entry2 *
ir_structure_splitting_visitor::get_splitting_entry(ir_variable *var)
{
	check(var);

	if (!var->type->is_record())
		return NULL;

	foreach_iter(exec_list_iterator, iter, *this->variable_list)
	{
		variable_entry2 *entry = (variable_entry2 *)iter.get();
		if (entry->var == var)
		{
			return entry;
		}
	}

	return NULL;
}

void
ir_structure_splitting_visitor::split_deref(ir_dereference **deref)
{
	if ((*deref)->ir_type != ir_type_dereference_record)
		return;

	ir_dereference_record *deref_record = (ir_dereference_record *)*deref;
	ir_dereference_variable *deref_var = deref_record->record->as_dereference_variable();
	if (!deref_var)
		return;

	variable_entry2 *entry = get_splitting_entry(deref_var->var);
	if (!entry)
		return;

	unsigned int i;
	for (i = 0; i < entry->var->type->length; i++)
	{
		if (strcmp(deref_record->field,
			entry->var->type->fields.structure[i].name) == 0)
			break;
	}
	check(i != entry->var->type->length);

	*deref = new(entry->mem_ctx) ir_dereference_variable(entry->components[i]);
}

void
ir_structure_splitting_visitor::handle_rvalue(ir_rvalue **rvalue)
{
	if (!*rvalue)
		return;

	ir_dereference *deref = (*rvalue)->as_dereference();

	if (!deref)
		return;

	split_deref(&deref);
	*rvalue = deref;
}

ir_visitor_status
ir_structure_splitting_visitor::visit_enter(ir_texture *ir)
{
	ir_dereference* DeRefState = ir->SamplerState ? ir->SamplerState->as_dereference() : NULL;
	if (DeRefState)
	{
		split_deref(&DeRefState);
		check(DeRefState->as_dereference());
		ir->SamplerState = DeRefState;
	}

	ir_dereference *deref = ir->sampler;
	split_deref(&deref);
	ir->sampler = deref;
	return visit_continue;
}

ir_visitor_status
ir_structure_splitting_visitor::visit_leave(ir_assignment *ir)
{
	ir_dereference_variable *lhs_deref = ir->lhs->as_dereference_variable();
	ir_dereference_variable *rhs_deref = ir->rhs->as_dereference_variable();
	variable_entry2 *lhs_entry = lhs_deref ? get_splitting_entry(lhs_deref->var) : NULL;
	variable_entry2 *rhs_entry = rhs_deref ? get_splitting_entry(rhs_deref->var) : NULL;
	const glsl_type *type = ir->rhs->type;

	if ((lhs_entry || rhs_entry) && !ir->condition)
	{
		for (unsigned int i = 0; i < type->length; i++)
		{
			ir_dereference *new_lhs, *new_rhs;
			void *mem_ctx = lhs_entry ? lhs_entry->mem_ctx : rhs_entry->mem_ctx;

			if (lhs_entry)
			{
				new_lhs = new(mem_ctx)ir_dereference_variable(lhs_entry->components[i]);
			}
			else
			{
				new_lhs = new(mem_ctx)
					ir_dereference_record(ir->lhs->clone(mem_ctx, NULL),
					type->fields.structure[i].name);
			}

			if (rhs_entry)
			{
				new_rhs = new(mem_ctx)ir_dereference_variable(rhs_entry->components[i]);
			}
			else
			{
				new_rhs = new(mem_ctx)
					ir_dereference_record(ir->rhs->clone(mem_ctx, NULL),
					type->fields.structure[i].name);
			}

			ir->insert_before(new(mem_ctx)ir_assignment(new_lhs,
				new_rhs,
				NULL));
		}
		ir->remove();
	}
	else
	{
		handle_rvalue(&ir->rhs);
		split_deref(&ir->lhs);
	}

	handle_rvalue(&ir->condition);

	return visit_continue;
}

bool do_structure_splitting(exec_list *instructions, _mesa_glsl_parse_state * state)
{
	ir_structure_reference_visitor refs;

	visit_list_elements(&refs, instructions);
	
	bool const bSplitInputVariables = state->LanguageSpec->SplitInputVariableStructs();

	/* Trim out variables we can't split. */
	foreach_iter(exec_list_iterator, iter, refs.variable_list)
	{
		variable_entry2 *entry = (variable_entry2 *)iter.get();

		if (debug)
		{
			printf("structure %s@%p: decl %d, whole_access %d\n",
				entry->var->name, (void *)entry->var, entry->declaration,
				entry->whole_structure_access);
		}

		if (!entry->declaration || entry->whole_structure_access || entry->var->is_interface_block)
		{
			entry->remove();
		}
		else if (!bSplitInputVariables && entry->var->mode == ir_var_in)
		{
			entry->remove();
		}
	}

	if (refs.variable_list.is_empty())
		return false;

	void *mem_ctx = ralloc_context(NULL);

	/* Replace the decls of the structures to be split with their split
	* components.
	*/
	foreach_iter(exec_list_iterator, iter, refs.variable_list)
	{
		variable_entry2 *entry = (variable_entry2 *)iter.get();
		const struct glsl_type *type = entry->var->type;

		entry->mem_ctx = ralloc_parent(entry->var);

		entry->components = ralloc_array(mem_ctx,
			ir_variable *,
			type->length);

		for (unsigned int i = 0; i < entry->var->type->length; i++)
		{
			const char *name = ralloc_asprintf(mem_ctx, "%s_%s",
				entry->var->name,
				type->fields.structure[i].name);

			// For platforms that don't normally split input variables we can cheat and reference the global resource directly
			// This doesn't work on platforms that require structure splitting
			ir_variable* var = state->symbols->get_variable(name);
			if (var && !bSplitInputVariables)
			{
				entry->components[i] = var;
			}
			else
			{
				entry->components[i] =
					new(state) ir_variable(type->fields.structure[i].type,
					name,
					ir_var_temporary);
				entry->var->insert_before(entry->components[i]);
			}
		}

		entry->var->remove();
	}

	ir_structure_splitting_visitor split(&refs.variable_list);
	visit_list_elements(&split, instructions);

	ralloc_free(mem_ctx);

	return true;
}
