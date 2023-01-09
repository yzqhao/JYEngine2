
#include "ShaderCompilerCommon.h"
#include "glsl_symbol_table.h"
#include "glsl_parser_extras.h"
#include "ast.h"
#include "glsl_types.h"
#include "hash_table.h"
#include "ir.h"
#include "ir_rvalue_visitor.h"
#include "macros.h"
#include "IRDump.h"
#include "LanguageSpec.h"

static void remove_matrix_swizzles(exec_list *instructions);

void _mesa_ast_to_hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	_mesa_glsl_initialize_functions(instructions, state);

	state->symbols->language_version = state->language_version;

	state->current_function = NULL;

	state->toplevel_ir = instructions;

	/* Section 4.2 of the GLSL 1.20 specification states:
	* "The built-in functions are scoped in a scope outside the global scope
	*  users declare global variables in.  That is, a shader's global scope,
	*  available for user-defined functions and global variables, is nested
	*  inside the scope containing the built-in functions."
	*
	* Since built-in functions like ftransform() access built-in variables,
	* it follows that those must be in the outer scope as well.
	*
	* We push scope here to create this nesting effect...but don't pop.
	* This way, a shader's globals are still in the symbol table for use
	* by the linker.
	*/
	state->symbols->push_scope();

	foreach_list_typed(ast_node, ast, link, &state->translation_unit)
	{
		ast->hir(instructions, state);
	}

	detect_recursion_unlinked(state, instructions);
	remove_matrix_swizzles(instructions);

	state->toplevel_ir = NULL;
}

bool do_scalar_conversion(const glsl_type *to, ir_rvalue * &from,
	exec_list* instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	check(from->type->is_scalar());

	// Scalar -> scalar.
	if (to->is_scalar())
	{
		from = convert_component(from, to);
		return true;
	}

	// Scalar -> vector.
	if (to->is_vector())
	{
		from = convert_component(from, to->get_base_type());
		from = new(ctx)ir_swizzle(from, 0, 0, 0, 0, to->components());
		return true;
	}

	// Scalar -> matrix.
	if (to->is_matrix())
	{
		from = convert_component(from, to->get_base_type());

		// Construct a temporary matrix.
		ir_variable* var = new(ctx)ir_variable(to, NULL, ir_var_temporary);
		instructions->push_tail(var);

		// Construct column vectors from the scalar and assign to the temp matrix.
		const glsl_type* column_type = to->column_type();
		for (unsigned i = 0; i < to->matrix_columns; ++i)
		{
			instructions->push_tail(
				new(ctx)ir_assignment(
				new(ctx)ir_dereference_array(var, new(ctx)ir_constant(i)),
				new(ctx)ir_swizzle(from->clone(ctx, NULL), 0, 0, 0, 0, column_type->components())
				)
				);
		}
		from = new(ctx)ir_dereference_variable(var);
		return true;
	}

	return false;
}

static ir_rvalue* access_typed_value_at_offset(
	ir_rvalue* value,
	unsigned value_offset,
	const glsl_type* desired_type)
{
	void *ctx = ralloc_parent(value);
	const glsl_type* type = value->type;

	if (type == desired_type)
	{
		return value;
	}
	else if (value_offset < type->component_slots())
	{
		if (type->is_record())
		{
			for (unsigned i = 0; i < type->length; ++i)
			{
				const glsl_type* field_type = type->fields.structure[i].type;
				unsigned field_slots = field_type->component_slots();
				if (value_offset == 0 && type->fields.structure[i].type == desired_type)
				{
					return new(ctx)ir_dereference_record(value, type->fields.structure[i].name);
				}
				else if (value_offset < field_slots)
				{
					return access_typed_value_at_offset(
						new(ctx)ir_dereference_record(value, type->fields.structure[i].name),
						value_offset,
						desired_type
						);
				}
				value_offset -= field_slots;
			}
		}
		else if (type->is_array())
		{
			unsigned slots_per_element = type->fields.array->component_slots();
			if ((value_offset % slots_per_element) == 0
				&& type->fields.array == desired_type)
			{
				return new(ctx)ir_dereference_array(value, new(ctx)ir_constant(value_offset / slots_per_element));
			}
			else
			{
				return access_typed_value_at_offset(
					new(ctx)ir_dereference_array(
					value,
					new(ctx)ir_constant(value_offset / slots_per_element)
					),
					value_offset % slots_per_element,
					desired_type
					);
			}
		}
		else if (type->is_matrix())
		{
			if ((value_offset % type->vector_elements) == 0
				&& type->row_type() == desired_type)
			{
				return new(ctx)ir_dereference_array(value, new(ctx)ir_constant(value_offset / type->vector_elements));
			}
			else if (desired_type->is_scalar())
			{
				return new(ctx)ir_dereference_array(
					new(ctx)ir_dereference_array(
					value,
					new(ctx)ir_constant(value_offset / type->vector_elements)
					),
					new(ctx)ir_constant(value_offset % type->vector_elements)
					);
			}
		}
		else if (type->is_vector() && desired_type->is_scalar())
		{
			return new(ctx)ir_dereference_array(value, new(ctx)ir_constant(value_offset));
		}
		else if ((type->is_scalar() || type->is_sampler() || type->IsSamplerState()) && desired_type->is_scalar())
		{
			return value;
		}
	}
	return NULL;
}

static ir_rvalue* access_value_at_offset(
	ir_rvalue* value,
	unsigned value_offset)
{
	void *ctx = ralloc_parent(value);
	const glsl_type* type = value->type;
	if (value_offset < type->component_slots())
	{
		if (type->is_record())
		{
			for (unsigned i = 0; i < type->length; ++i)
			{
				const glsl_type* field_type = type->fields.structure[i].type;
				unsigned field_slots = field_type->component_slots();
				if (value_offset < field_slots)
				{
					return access_value_at_offset(
						new(ctx)ir_dereference_record(value, type->fields.structure[i].name),
						value_offset
						);
				}
				value_offset -= field_slots;
			}
		}
		else if (type->is_array())
		{
			unsigned slots_per_element = type->fields.array->component_slots();
			return access_value_at_offset(
				new(ctx)ir_dereference_array(
				value,
				new(ctx)ir_constant(value_offset / slots_per_element)
				),
				value_offset % slots_per_element
				);
		}
		else if (type->is_matrix())
		{
			return new(ctx)ir_dereference_array(
				new(ctx)ir_dereference_array(
				value,
				new(ctx)ir_constant(value_offset / type->vector_elements)
				),
				new(ctx)ir_constant(value_offset % type->vector_elements)
				);
		}
		else if (type->is_vector())
		{
			return new(ctx)ir_dereference_array(value, new(ctx)ir_constant(value_offset));
		}
		else if (type->is_scalar() || type->is_sampler() || type->IsSamplerState())
		{
			return value;
		}
	}
	return NULL;
}

static unsigned intialize_struct(
	exec_list *instructions,
	struct _mesa_glsl_parse_state *state,
	ir_dereference *struct_dref,
	ir_rvalue *value,
	unsigned value_offset);

static unsigned intialize_array(
	exec_list *instructions,
	struct _mesa_glsl_parse_state *state,
	ir_dereference *array_dref,
	ir_rvalue *value,
	unsigned value_offset);

static unsigned do_initialize(
	exec_list *instructions,
	struct _mesa_glsl_parse_state *state,
	ir_dereference *dref,
	ir_rvalue *value,
	unsigned value_offset)
{
	if (dref->type->is_record())
	{
		value_offset = intialize_struct(instructions, state, dref, value, value_offset);
	}
	else if (dref->type->is_array())
	{
		value_offset = intialize_array(instructions, state, dref, value, value_offset);
	}
	else
	{
		void *ctx = state;
		const glsl_type* rhs_type = value->type;
		if (rhs_type->is_scalar())
		{
			ir_rvalue* rhs = value->clone(ctx, NULL);
			bool converted = do_scalar_conversion(dref->type, rhs, instructions, state);
			check(converted);
			instructions->push_tail(new(ctx)ir_assignment(dref, rhs));
			value_offset += dref->type->component_slots();
		}
		else
		{
			unsigned num_components = dref->type->component_slots();
			for (unsigned j = 0; j < num_components; ++j)
			{
				ir_rvalue* lhs = access_value_at_offset(dref->clone(ctx, NULL), j);
				ir_rvalue* rhs = access_value_at_offset(value->clone(ctx, NULL), value_offset + j);
				instructions->push_tail(new(ctx)ir_assignment(lhs, convert_component(rhs, lhs->type)));
			}
			value_offset += num_components;
		}
	}

	return value_offset;
}

static unsigned intialize_struct(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	ir_dereference *struct_dref,
	ir_rvalue *value,
	unsigned value_offset)
{
	void *ctx = state;
	const glsl_type *type = struct_dref->type;
	check(type->base_type == GLSL_TYPE_STRUCT);

	for (unsigned i = 0; i < type->length; i++)
	{
		ir_dereference *lhs = new(ctx)ir_dereference_record(
			struct_dref->clone(ctx, NULL),
			type->fields.structure[i].name);
		value_offset = do_initialize(instructions, state, lhs, value, value_offset);
	}

	return value_offset;
}

static unsigned intialize_array(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	ir_dereference *array_dref,
	ir_rvalue *value,
	unsigned value_offset)
{
	void *ctx = state;
	check(array_dref->type->base_type == GLSL_TYPE_ARRAY);

	for (unsigned i = 0; i < array_dref->type->length; i++)
	{
		ir_dereference *lhs =
			new(ctx)ir_dereference_array(array_dref->clone(ctx, NULL),
			new(ctx)ir_constant((unsigned)i));
		value_offset = do_initialize(instructions, state, lhs, value, value_offset);
	}

	return value_offset;
}

static ir_dereference_variable* make_aggregate(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	const glsl_type *type,
	ir_rvalue *value)
{
	unsigned src_components = value->type->component_slots();
	unsigned dest_components = type->component_slots();
	if (value->type->is_scalar() || dest_components <= src_components)
	{
		if (!(value->type->is_matrix() ^ type->is_matrix()) || dest_components == src_components)
		{
			// If the value is not a scalar, store it in a temporary variable.
			if (!value->type->is_scalar() && value->as_dereference() == NULL)
			{
				ir_variable* var = new(state)ir_variable(value->type, NULL, ir_var_temporary);
				ir_dereference_variable* dref = new(state)ir_dereference_variable(var);
				instructions->push_tail(var);
				instructions->push_tail(new(state)ir_assignment(dref, value));
				value = dref->clone(state, NULL);
			}

			ir_variable* var = new(state)ir_variable(type, NULL, ir_var_temporary);
			ir_dereference_variable* dref = new(state)ir_dereference_variable(var);
			instructions->push_tail(var);
			unsigned num_values = do_initialize(instructions, state, dref, value, 0);
			check(num_values == type->component_slots());
			return dref->clone(state, NULL);
		}
	}
	return NULL;
}

/**
* If a conversion is available, convert one operand to a different type
*
* The \c from \c ir_rvalue is converted "in place".
*
* \param to     Type that the operand it to be converted to
* \param from   Operand that is being converted
* \param state  GLSL compiler state
*
* \return
* If a conversion is possible (or unnecessary), \c true is returned.
* Otherwise \c false is returned.
*/
bool apply_type_conversion(
	const glsl_type *to, ir_rvalue * &from,
	exec_list* instructions, struct _mesa_glsl_parse_state *state,
	bool is_explicit, const YYLTYPE *loc)
{
	void *ctx = state;

	// Trivial if the types are the same: no conversion.
	if (to == from->type)
	{
		return true;
	}

	// Scalar -> scalar, vector, or matrix.
	if (from->type->is_scalar() && do_scalar_conversion(to, from, instructions, state))
	{
		return true;
	}

	// Scalar/Vector/Matrix/Structure -> structure, requires explicit cast.
	if (is_explicit && (to->base_type == GLSL_TYPE_STRUCT || from->type->base_type == GLSL_TYPE_STRUCT))
	{
		ir_dereference_variable* temp_struct = make_aggregate(instructions, state, to, from);
		if (temp_struct)
		{
			from = temp_struct;
			return true;
		}
		return false;
	}

	// Vector -> vector, same size.
	if (from->type->is_vector() && to->is_vector() &&
		from->type->components() == to->components())
	{
		from = convert_component(from, to);
		return true;
	}

	// Vector -> vector, larger to smaller. Also handles vector -> scalar.
	if (from->type->is_vector() && (to->is_vector() || to->is_scalar()) &&
		from->type->components() > to->components())
	{
		if (is_explicit == false)
		{
			_mesa_glsl_warning(loc, state, "implicit truncation from '%s' to '%s'",
				from->type->name, to->name);
		}

		from = new(ctx)ir_swizzle(from, 0, 1, 2, 3, to->components());
		from = convert_component(from, to);
		return true;
	}

	// Vector -> matrix, same size.
	if (from->type->is_vector() && to->is_matrix() &&
		from->type->components() == to->components())
	{
		ir_dereference_variable* temp_matrix = make_aggregate(instructions, state, to, from);
		if (temp_matrix)
		{
			from = temp_matrix;
			return true;
		}
		return false;
	}

	// Matrix -> scalar.
	if (from->type->is_matrix() && to->is_scalar())
	{
		if (is_explicit == false)
		{
			_mesa_glsl_warning(loc, state, "implicit truncation from '%s' to '%s'",
				from->type->name, to->name);
		}

		from = new(state)ir_dereference_array(
			new(state)ir_dereference_array(from, new(state)ir_constant((unsigned)0)),
			new(state)ir_constant((unsigned)0)
			);
		from = convert_component(from, to);
		return true;
	}

	// Matrix -> vector, same size.
	if (from->type->is_matrix() && to->is_vector() &&
		from->type->components() == to->components())
	{
		ir_dereference_variable* temp_vector = make_aggregate(instructions, state, to, from);
		if (temp_vector)
		{
			from = temp_vector;
			return true;
		}
		return false;
	}

	// Matrix -> matrix, same size.
	if (state->LanguageSpec->SupportsMatrixConversions() && from->type->is_matrix() && to->is_matrix() &&
		from->type->matrix_columns == to->matrix_columns &&
		from->type->vector_elements == to->vector_elements)
	{
		from = convert_component(from, to);
		return true;
	}

	// Matrix -> matrix, larger to smaller.
	if (from->type->is_matrix() && to->is_matrix() &&
		from->type->matrix_columns >= to->matrix_columns &&
		from->type->vector_elements >= to->vector_elements)
	{
		if (is_explicit == false)
		{
			if (from->type->base_type == GLSL_TYPE_HALF && to->base_type == GLSL_TYPE_FLOAT)
			{
				// Don't warn going from half to float
			}
			else
			{
				_mesa_glsl_warning(loc, state, "implicit truncation from '%s' to '%s'",
					(from->type->HlslName ? from->type->HlslName : from->type->name),
					to->HlslName ? to->HlslName : to->name);
			}
		}

		ir_variable* var = new(state)ir_variable(to, NULL, ir_var_temporary);
		ir_dereference_variable* dref = new(state)ir_dereference_variable(var);
		instructions->push_tail(var);

		for (unsigned i = 0; i < to->matrix_columns; ++i)
		{
			ir_dereference* lhs = new(state)ir_dereference_array(
				dref->clone(state, NULL),
				new(state)ir_constant(i));
			ir_rvalue* rhs = new(state)ir_swizzle(
				new(state)ir_dereference_array(from->clone(state, NULL), new(state)ir_constant(i)),
				0, 1, 2, 3, to->vector_elements);
			rhs = convert_component(rhs, lhs->type);
			instructions->push_tail(new(state)ir_assignment(lhs, rhs));
		}
		from = dref;
		return true;
	}

	return false;
}

static bool make_types_compatible(ir_rvalue* &value_a, ir_rvalue* &value_b,
	exec_list* instructions, _mesa_glsl_parse_state* state, YYLTYPE* loc, bool bAIsLHS)
{
	glsl_base_type base_type = GLSL_TYPE_ERROR;
	const glsl_type* type_a = value_a->type;
	const glsl_type* type_b = value_b->type;

	if (type_a == type_b)
	{
		return true;
	}

	if ((!type_a->is_numeric() && !type_a->is_boolean()) ||
		(!type_b->is_numeric() && !type_b->is_boolean()))
	{
		return false;
	}

	// Determine the preferred type for these two values.
	if (bAIsLHS)
	{
		// If dealing with the LHS of an assignment, coerce the RHS to match its type
		base_type = type_a->base_type;
	}
	else
	{
		// Handle the case of k OP x, where k is a literal and x is non-const, so we can make 'k' the proper type; this is only needed
		// when k is a float constant, as we need to cast it to either a float or a half depending on context
		auto* ConstA = value_a->constant_expression_value();
		auto* ConstB = value_b->constant_expression_value();
		if ((ConstA != nullptr) ^ (ConstB != nullptr))
		{
			if (ConstA && ConstA->type->is_float() && type_b->is_float())
			{
				base_type = type_b->base_type;
			}
			else if (ConstB && ConstB->type->is_float() && type_a->is_float())
			{
				base_type = type_a->base_type;
			}
		}

		if (base_type == GLSL_TYPE_ERROR)
		{
			// Otherwise
			//	float > half > uint > int > bool

			// Proceed with regular conversions
			if (type_a->base_type == GLSL_TYPE_FLOAT || type_b->base_type == GLSL_TYPE_FLOAT)
			{
				base_type = GLSL_TYPE_FLOAT;
			}
			else if (type_a->base_type == GLSL_TYPE_HALF || type_b->base_type == GLSL_TYPE_HALF)
			{
				base_type = GLSL_TYPE_HALF;
			}
			else if (type_a->base_type == GLSL_TYPE_UINT || type_b->base_type == GLSL_TYPE_UINT)
			{
				base_type = GLSL_TYPE_UINT;
			}
			else if (type_a->base_type == GLSL_TYPE_INT || type_b->base_type == GLSL_TYPE_INT)
			{
				base_type = GLSL_TYPE_INT;
			}
			else if (type_a->base_type == GLSL_TYPE_BOOL || type_b->base_type == GLSL_TYPE_BOOL)
			{
				base_type = GLSL_TYPE_BOOL;
			}
		}
	}

	if (base_type == GLSL_TYPE_ERROR)
	{
		return false;
	}

	// Determine how many rows and columns to use.
	unsigned rows = 0, cols = 0;
	if (bAIsLHS)
	{
		// If LHS is an l-value of an assignment, LHS dictates the type dimension.
		// Otherwise, swizzle operators might be generated that make it an r-value,
		// which is not allowed on the left hand side of an assignment!
		rows = type_a->vector_elements;
		cols = type_a->matrix_columns;
	}
	else
	{
		if (type_a->is_scalar() || type_b->is_scalar())
		{
			rows = MAX2(type_a->vector_elements, type_b->vector_elements);
			cols = MAX2(type_a->matrix_columns, type_b->matrix_columns);
		}
		else if (type_a->components() > type_b->components())
		{
			rows = type_b->vector_elements;
			cols = type_b->matrix_columns;
		}
		else
		{
			rows = type_a->vector_elements;
			cols = type_a->matrix_columns;
		}
	}

	// Now we know the desired type, try to convert.
	const glsl_type* desired_type = glsl_type::get_instance(base_type, rows, cols);
	return (!desired_type->is_error() &&
		apply_type_conversion(desired_type, value_a, instructions, state, false, loc) &&
		apply_type_conversion(desired_type, value_b, instructions, state, false, loc));
}

const struct glsl_type* arithmetic_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc, bool bAIsLHS)
{
	const glsl_type* type_a = value_a->type;
	const glsl_type* type_b = value_b->type;

	if (make_types_compatible(value_a, value_b, instructions, state, loc, bAIsLHS))
	{
		return value_a->type;
	}

	_mesa_glsl_error(loc, state, "cannot implicitly convert '%s' and '%s' in arithmetic expression",
		type_a->name, type_b->name);
	return glsl_type::error_type;
}


static const struct glsl_type * unary_arithmetic_result_type(
	ir_rvalue* &value, ast_operators op, exec_list* instructions,
	_mesa_glsl_parse_state *state, YYLTYPE *loc)
{
	if (!value->type->is_numeric())
	{
		if (op == ast_neg && value->type->is_boolean())
		{
			const glsl_type* result_type = glsl_type::get_instance(
				GLSL_TYPE_INT, value->type->vector_elements, value->type->matrix_columns);
			if (apply_type_conversion(result_type, value, instructions, state, false, loc))
			{
				return result_type;
			}
		}

		_mesa_glsl_error(loc, state, "Operands to arithmetic operators must be numeric");
		return glsl_type::error_type;
	}

	return value->type;
}

/**
* \brief Return the result type of a bit-logic operation.
*
* If the given types to the bit-logic operator are invalid, return
* glsl_type::error_type.
*
* \param type_a Type of LHS of bit-logic op
* \param type_b Type of RHS of bit-logic op
*/
static const struct glsl_type * bit_logic_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc, bool bAIsLHS)
{
	const glsl_type *type_a = value_a->type;
	const glsl_type *type_b = value_b->type;

	if ((!type_a->is_integer() && !type_a->is_boolean()) ||
		(!type_b->is_integer() && !type_b->is_boolean()))
	{
		_mesa_glsl_error(loc, state, "bitwise operator only valid for integral types");
		return glsl_type::error_type;
	}

	if (make_types_compatible(value_a, value_b, instructions, state, loc, bAIsLHS))
	{
		return value_a->type;
	}

	_mesa_glsl_error(loc, state, "Could not implicitly convert operands to "
		"bitwise operator: '%s' and '%s'", type_a->name, type_b->name);
	return glsl_type::error_type;
}

static const struct glsl_type * modulus_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc, bool bAIsLHS)
{
	if (!state->LanguageSpec->SupportsIntegerModulo() && (value_a->type->is_integer() || value_b->type->is_integer()))
	{
		_mesa_glsl_error(loc, state, "Integer modulo/remainder is not an allowed operation on ES 2");
	}
	// HLSL treats the modulus operator like any other arithmetic operator.
	return arithmetic_result_type(value_a, value_b, instructions, state, loc, bAIsLHS);
}


static const struct glsl_type* relational_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc)
{
	const glsl_type *type_a = value_a->type;
	const glsl_type *type_b = value_b->type;

	if (make_types_compatible(value_a, value_b, instructions, state, loc, false))
	{
		const glsl_type* result_type = glsl_type::get_instance(
			GLSL_TYPE_BOOL,
			value_a->type->vector_elements,
			value_a->type->matrix_columns);
		return result_type;
	}

	_mesa_glsl_error(loc, state, "Could not implicitly convert operands to "
		"relational operator: '%s' and '%s'", type_a->name, type_b->name);
	return glsl_type::error_type;
}

static const struct glsl_type * binary_logic_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc)
{
	const glsl_type *type_a = value_a->type;
	const glsl_type *type_b = value_b->type;

	if (make_types_compatible(value_a, value_b, instructions, state, loc, false))
	{
		const glsl_type* result_type = glsl_type::get_instance(
			GLSL_TYPE_BOOL,
			value_a->type->vector_elements,
			value_a->type->matrix_columns);
		if (apply_type_conversion(result_type, value_a, instructions, state, false, loc) &&
			apply_type_conversion(result_type, value_b, instructions, state, false, loc))
		{
			return result_type;
		}
	}

	_mesa_glsl_error(loc, state, "Could not implicitly convert operands to "
		"logical operator: '%s' and '%s'", type_a->name, type_b->name);
	return glsl_type::error_type;
}

/**
* \brief Return the result type of a bit-shift operation.
*
* If the given types to the bit-shift operator are invalid, return
* glsl_type::error_type.
*
* \param type_a Type of LHS of bit-shift op
* \param type_b Type of RHS of bit-shift op
*/
static const struct glsl_type* shift_result_type(ir_rvalue * &value_a, ir_rvalue * &value_b,
	exec_list *instructions, _mesa_glsl_parse_state *state, YYLTYPE *loc, bool bAIsLHS)
{
	const glsl_type *type_a = value_a->type;
	const glsl_type *type_b = value_b->type;

	if (!type_a->is_integer() && !type_a->is_boolean())
	{
		_mesa_glsl_error(loc, state, "LHS of shift operator must be an integer or "
			"integer vector");
		return glsl_type::error_type;
	}
	if (!type_b->is_integer() && !type_b->is_boolean())
	{
		_mesa_glsl_error(loc, state, "RHS of shift operator must be an integer or "
			"integer vector");
		return glsl_type::error_type;
	}

	// Determine the desired type.
	glsl_base_type base_type = type_a->base_type == GLSL_TYPE_BOOL ? GLSL_TYPE_INT : type_a->base_type;
	unsigned rows = type_a->vector_elements;
	unsigned cols = type_a->matrix_columns;

	if (type_a->is_scalar()
		|| (!type_b->is_scalar() && type_a->components() > type_b->components()))
	{
		rows = type_b->vector_elements;
		cols = type_b->matrix_columns;
	}
	//@todo-rco bAIsLHS
	const glsl_type* result_type = glsl_type::get_instance(base_type, rows, cols);
	if (apply_type_conversion(result_type, value_a, instructions, state, false, loc) &&
		apply_type_conversion(result_type, value_b, instructions, state, false, loc))
	{
		return result_type;
	}

	_mesa_glsl_error(loc, state, "Could not implicitly convert operands to "
		"shift operator: '%s' and '%s'", type_a->name, type_b->name);
	return glsl_type::error_type;
}

/**
* Validates that a value can be assigned to a location with a specified type
*
* Validates that \c rhs can be assigned to some location.  If the types are
* not an exact match but an automatic conversion is possible, \c rhs will be
* converted.
*
* \return
* \c NULL if \c rhs cannot be assigned to a location with type \c lhs_type.
* Otherwise the actual RHS to be assigned will be returned.  This may be
* \c rhs, or it may be \c rhs after some type conversion.
*
* \note
* In addition to being used for assignments, this function is used to
* type-check return values.
*/
ir_rvalue * validate_assignment(struct _mesa_glsl_parse_state *state,
	exec_list *instructions, const glsl_type *lhs_type, ir_rvalue *rhs, bool is_initializer, const YYLTYPE *loc)
{
	/* If there is already some error in the RHS, just return it.  Anything
	* else will lead to an avalanche of error message back to the user.
	*/
	if (rhs->type->is_error())
	{
		return rhs;
	}

	/* If the types are identical, the assignment can trivially proceed.
	*/
	if (rhs->type == lhs_type)
	{
		return rhs;
	}

	/* If the array element types are the same and the size of the LHS is zero,
	* the assignment is okay for initializers embedded in variable
	* declarations.
	*
	* Note: Whole-array assignments are not permitted in GLSL 1.10, but this
	* is handled by ir_dereference::is_lvalue.
	*/
	if (is_initializer && lhs_type->is_array() && rhs->type->is_array()
		&& (lhs_type->element_type() == rhs->type->element_type())
		&& (lhs_type->array_size() == 0))
	{
		return rhs;
	}

	/* Check for implicit conversion in GLSL 1.20 */
	if (apply_type_conversion(lhs_type, rhs, instructions, state, false, loc))
	{
		if (rhs->type == lhs_type)
		{
			return rhs;
		}
	}

	return NULL;
}

static void mark_whole_array_access(ir_rvalue *access)
{
	ir_dereference_variable *deref = access->as_dereference_variable();

	if (deref && deref->var)
	{
		deref->var->max_array_access = deref->type->length - 1;
	}
}

ir_rvalue* do_assignment(exec_list *instructions, struct _mesa_glsl_parse_state *state,
	const char *non_lvalue_description, ir_rvalue *lhs, ir_rvalue *rhs, bool is_initializer, YYLTYPE lhs_loc)
{
	void *ctx = state;
	bool error_emitted = (lhs->type->is_error() || rhs->type->is_error());

	if (!error_emitted)
	{
		if (non_lvalue_description != NULL)
		{
			_mesa_glsl_error(&lhs_loc, state,
				"assignment to %s",
				non_lvalue_description);
			error_emitted = true;
		}
		else if (lhs->variable_referenced() != NULL
			&& lhs->variable_referenced()->read_only)
		{
			_mesa_glsl_error(&lhs_loc, state,
				"assignment to read-only variable '%s'",
				lhs->variable_referenced()->name);
			error_emitted = true;

		}
		else if (state->language_version <= 110 && lhs->type->is_array())
		{
			/* From page 32 (page 38 of the PDF) of the GLSL 1.10 spec:
			*
			*    "Other binary or unary expressions, non-dereferenced
			*     arrays, function names, swizzles with repeated fields,
			*     and constants cannot be l-values."
			*/
			_mesa_glsl_error(&lhs_loc, state, "whole array assignment is not "
				"allowed in GLSL 1.10 or GLSL ES 1.00.");
			error_emitted = true;
		}
		else if (!lhs->is_lvalue())
		{
			_mesa_glsl_error(&lhs_loc, state, "non-lvalue in assignment");
			error_emitted = true;
		}
	}

	ir_rvalue *new_rhs = validate_assignment(state, instructions, lhs->type, rhs, is_initializer, &lhs_loc);
	if (new_rhs == NULL)
	{
		_mesa_glsl_error(&lhs_loc, state, "cannot assign value of type '%s' to type '%s': no implicit conversion allowed",
			rhs->type->name, lhs->type->name);
	}
	else
	{
		rhs = new_rhs;

		/* If the LHS array was not declared with a size, it takes it size from
		* the RHS.  If the LHS is an l-value and a whole array, it must be a
		* dereference of a variable.  Any other case would require that the LHS
		* is either not an l-value or not a whole array.
		*/
		if (lhs->type->array_size() == 0)
		{
			ir_dereference *const d = lhs->as_dereference();

			check(d != NULL);

			ir_variable *const var = d->variable_referenced();

			check(var != NULL);

			if (var->max_array_access >= unsigned(rhs->type->array_size()))
			{
				/* FINISHME: This should actually log the location of the RHS. */
				_mesa_glsl_error(&lhs_loc, state, "array size must be > %u due to "
					"previous access",
					var->max_array_access);
			}

			var->type = glsl_type::get_array_instance(lhs->type->element_type(),
				rhs->type->array_size());
			d->type = var->type;
		}
		mark_whole_array_access(rhs);
		mark_whole_array_access(lhs);
	}

	/* Most callers of do_assignment (assign, add_assign, pre_inc/dec,
	* but not post_inc) need the converted assigned value as an rvalue
	* to handle things like:
	*
	* i = j += 1;
	*
	* So we always just store the computed value being assigned to a
	* temporary and return a deref of that temporary.  If the rvalue
	* ends up not being used, the temp will get copy-propagated out.
	*/
	ir_variable *var = new(ctx)ir_variable(rhs->type, "assign_tmp", ir_var_temporary);
	ir_dereference_variable *deref_var = new(ctx)ir_dereference_variable(var);
	instructions->push_tail(var);
	instructions->push_tail(new(ctx)ir_assignment(deref_var,
		rhs,
		NULL));
	deref_var = new(ctx)ir_dereference_variable(var);

	if (!error_emitted)
	{
		instructions->push_tail(new(ctx)ir_assignment(lhs, deref_var, NULL));
	}

	return new(ctx)ir_dereference_variable(var);
}

static ir_rvalue * get_lvalue_copy(exec_list *instructions, ir_rvalue *lvalue)
{
	void *ctx = ralloc_parent(lvalue);
	ir_variable *var;

	var = new(ctx)ir_variable(lvalue->type, "_post_incdec_tmp",
		ir_var_temporary);
	instructions->push_tail(var);

	instructions->push_tail(new(ctx)ir_assignment(new(ctx)ir_dereference_variable(var),
		lvalue, NULL));

	return new(ctx)ir_dereference_variable(var);
}


ir_rvalue * ast_node::hir(exec_list *instructions, _mesa_glsl_parse_state *state)
{
	(void)instructions;
	(void)state;

	return NULL;
}

/**
* Create the constant 1, of a which is appropriate for incrementing and
* decrementing values of the given GLSL type.  For example, if type is vec4,
* this creates a constant value of 1.0 having type float.
*
* If the given type is invalid for increment and decrement operators, return
* a floating point 1--the error will be detected later.
*/
static ir_rvalue * constant_one_for_inc_dec(void *ctx, const glsl_type *type)
{
	switch (type->base_type)
	{
	case GLSL_TYPE_UINT:
		return new(ctx)ir_constant((unsigned)1);
	case GLSL_TYPE_INT:
		return new(ctx)ir_constant(1);
	default:
	case GLSL_TYPE_HALF:
	case GLSL_TYPE_FLOAT:
		return new(ctx)ir_constant(1.0f);
	}
}

/**
* Checks an expr for NaNs. If the expression is a division it will also detect
* division by zero. Returns ir_rvalue::error_value if a NaN will be generated
* by this expression, otherwise the expression itself is returned.
*/
static ir_rvalue* check_expr_for_nan(
	YYLTYPE *loc,
	_mesa_glsl_parse_state *state,
	ir_expression *expr
	)
{
	bool division_by_zero = false;

	if (expr->operation == ir_binop_div)
	{
		ir_constant* const_denom = expr->operands[1]->constant_expression_value();
		if (const_denom && const_denom->are_any_zero())
		{
			division_by_zero = true;
		}
	}

	if (!division_by_zero)
	{
		ir_constant* const_expr = expr->constant_expression_value();
		if (const_expr && !const_expr->is_finite())
		{
			division_by_zero = true;
		}
	}

	if (division_by_zero)
	{
		if (expr->type->is_integer())
		{
			_mesa_glsl_error(loc, state, "integer division by zero");
			return ir_rvalue::error_value(state);
		}
		else
		{
			_mesa_glsl_warning(loc, state, "division by zero");
		}
	}
	return expr;
}

ir_rvalue *ast_expression::initializer_hir(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	const glsl_type *initializer_type)
{
	return this->hir(instructions, state);
}

ir_rvalue* ast_expression::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;
	static const int operations[AST_NUM_OPERATORS] =
	{
		-1,               /* ast_assign doesn't convert to ir_expression. */
		-1,               /* ast_plus doesn't convert to ir_expression. */
		ir_unop_neg,
		ir_binop_add,
		ir_binop_sub,
		ir_binop_mul,
		ir_binop_div,
		ir_binop_mod,
		ir_binop_lshift,
		ir_binop_rshift,
		ir_binop_less,
		ir_binop_greater,
		ir_binop_lequal,
		ir_binop_gequal,
		ir_binop_equal, // ir_binop_all_equal,
		ir_binop_nequal, // ir_binop_any_nequal,
		ir_binop_bit_and,
		ir_binop_bit_xor,
		ir_binop_bit_or,
		ir_unop_bit_not,
		ir_binop_logic_and,
		ir_binop_logic_xor,
		ir_binop_logic_or,
		ir_unop_logic_not,

		/* Note: The following block of expression types actually convert
		* to multiple IR instructions.
		*/
		ir_binop_mul,     /* ast_mul_assign */
		ir_binop_div,     /* ast_div_assign */
		ir_binop_mod,     /* ast_mod_assign */
		ir_binop_add,     /* ast_add_assign */
		ir_binop_sub,     /* ast_sub_assign */
		ir_binop_lshift,  /* ast_ls_assign */
		ir_binop_rshift,  /* ast_rs_assign */
		ir_binop_bit_and, /* ast_and_assign */
		ir_binop_bit_xor, /* ast_xor_assign */
		ir_binop_bit_or,  /* ast_or_assign */

		-1,               /* ast_conditional doesn't convert to ir_expression. */
		ir_binop_add,     /* ast_pre_inc. */
		ir_binop_sub,     /* ast_pre_dec. */
		ir_binop_add,     /* ast_post_inc. */
		ir_binop_sub,     /* ast_post_dec. */
		-1,               /* ast_field_selection doesn't conv to ir_expression. */
		-1,               /* ast_array_index doesn't convert to ir_expression. */
		-1,               /* ast_function_call doesn't conv to ir_expression. */
		-1,				/* ast_initializer_list doesn't conv to ir_expression. */
		-1,               /* ast_identifier doesn't convert to ir_expression. */
		-1,               /* ast_int_constant doesn't convert to ir_expression. */
		-1,               /* ast_uint_constant doesn't conv to ir_expression. */
		-1,               /* ast_float_constant doesn't conv to ir_expression. */
		-1,               /* ast_bool_constant doesn't conv to ir_expression. */
		-1,               /* ast_sequence doesn't convert to ir_expression. */
		-1,				/* ast_type_cast doesn't convert 1:1 to ir_expression. */
	};
	ir_rvalue *result = NULL;
	ir_rvalue *op[3];
	const struct glsl_type *type; /* a temporary variable for switch cases */
	bool error_emitted = false;
	YYLTYPE loc = this->get_location();
	switch (this->oper)
	{
	case ast_assign:
	{
					   op[0] = this->subexpressions[0]->hir(instructions, state);
					   op[1] = this->subexpressions[1]->hir(instructions, state);

					   result = do_assignment(instructions, state,
						   this->subexpressions[0]->non_lvalue_description,
						   op[0], op[1], false,
						   this->subexpressions[0]->get_location());
					   error_emitted = result->type->is_error();
					   break;
	}

	case ast_plus:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		type = unary_arithmetic_result_type(op[0], this->oper, instructions, state, &loc);
		error_emitted = type->is_error();
		result = op[0];
		break;

	case ast_neg:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		type = unary_arithmetic_result_type(op[0], this->oper, instructions, state, &loc);
		error_emitted = type->is_error();
		result = new(ctx)ir_expression(operations[this->oper], type,
			op[0], NULL);
		break;

	case ast_add:
	case ast_sub:
	case ast_mul:
	case ast_div:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		
		bool const bNativeMatrixIntrinsics = state->LanguageSpec->SupportsMatrixIntrinsics();
		if (bNativeMatrixIntrinsics && (this->oper == ast_mul && op[0]->type->is_vector() && op[1]->type->is_matrix()))
		{
			type = op[1]->type->column_type();
		}
		else
		{
			type = arithmetic_result_type(op[0], op[1], instructions, state, &loc, false);
		}
		ir_expression* expr = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		result = check_expr_for_nan(&loc, state, expr);
		error_emitted = result->type->is_error();
		break;
	}

	case ast_mod:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);

		type = modulus_result_type(op[0], op[1], instructions, state, &loc, false);
		check(operations[this->oper] == ir_binop_mod);

		result = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		error_emitted = type->is_error();
		break;

	case ast_lshift:
	case ast_rshift:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		type = shift_result_type(op[0], op[1], instructions, state, &loc, false);
		result = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();
		break;

	case ast_less:
	case ast_greater:
	case ast_lequal:
	case ast_gequal:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);

		type = relational_result_type(op[0], op[1], instructions, state, &loc);

		result = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		error_emitted = type->is_error();
		break;

	case ast_nequal:
	case ast_equal:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);

		type = relational_result_type(op[0], op[1], instructions, state, &loc);
		error_emitted = type->is_error();

		if (error_emitted)
		{
			result = new(ctx)ir_constant(false);
		}
		else
		{
			result = new(ctx)ir_expression(operations[this->oper], op[0], op[1]);
			check(result->type->base_type == GLSL_TYPE_BOOL);
		}
		break;

	case ast_bit_and:
	case ast_bit_xor:
	case ast_bit_or:
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		type = bit_logic_result_type(op[0], op[1], instructions, state, &loc, false);
		result = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();
		break;

	case ast_bit_not:
		op[0] = this->subexpressions[0]->hir(instructions, state);

		if (state->language_version < 130)
		{
			_mesa_glsl_error(&loc, state, "bit-wise operations require GLSL 1.30");
			error_emitted = true;
		}

		if (!op[0]->type->is_integer())
		{
			_mesa_glsl_error(&loc, state, "operand of '~' must be an integer");
			error_emitted = true;
		}

		type = error_emitted ? glsl_type::error_type : op[0]->type;
		result = new(ctx)ir_expression(ir_unop_bit_not, type, op[0], NULL);
		break;

	case ast_logic_and:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		YYLTYPE loc = this->get_location();
		type = binary_logic_result_type(op[0], op[1], instructions, state, &loc);
		result = new(ctx)ir_expression(ir_binop_logic_and, op[0], op[1]);
		check(type == result->type);
		break;
	}

	case ast_logic_or:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		YYLTYPE loc = this->get_location();
		type = binary_logic_result_type(op[0], op[1], instructions, state, &loc);
		result = new(ctx)ir_expression(ir_binop_logic_or, op[0], op[1]);
		check(type == result->type);
		break;
	}

	case ast_logic_xor:
	{
		check(0);
		result = ir_rvalue::error_value(ctx);
		type = result->type;
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state, "logic xor operator '^^' not supported");
		error_emitted = true;
	}
		break;

	case ast_logic_not:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		type = glsl_type::get_instance(GLSL_TYPE_BOOL, op[0]->type->vector_elements,
			op[0]->type->matrix_columns);
		YYLTYPE loc = this->get_location();
		if (apply_type_conversion(type, op[0], instructions, state, false, &loc))
		{
			result = new(ctx)ir_expression(ir_unop_logic_not, op[0]);
		}
		else
		{
			error_emitted = true;
			result = ir_rvalue::error_value(ctx);
			type = result->type;
		}
	}
		break;

	case ast_mul_assign:
	case ast_div_assign:
	case ast_add_assign:
	case ast_sub_assign:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);

		type = arithmetic_result_type(op[0], op[1], instructions, state, &loc, true);

		ir_expression *expr = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);
		ir_rvalue *temp_rhs = check_expr_for_nan(&loc, state, expr);

		result = do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());
		error_emitted = (op[0]->type->is_error());
		break;
	}

	case ast_mod_assign:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);

		type = modulus_result_type(op[0], op[1], instructions, state, &loc, true);

		check(operations[this->oper] == ir_binop_mod);

		ir_rvalue *temp_rhs;
		temp_rhs = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);

		result = do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());
		error_emitted = type->is_error();
		break;
	}

	case ast_ls_assign:
	case ast_rs_assign:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		type = shift_result_type(op[0], op[1], instructions, state, &loc, true);
		ir_rvalue *temp_rhs = new(ctx)ir_expression(operations[this->oper],
			type, op[0], op[1]);
		result = do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());
		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();
		break;
	}

	case ast_and_assign:
	case ast_xor_assign:
	case ast_or_assign:
	{
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		type = bit_logic_result_type(op[0], op[1], instructions, state, &loc, true);
		ir_rvalue *temp_rhs = new(ctx)ir_expression(operations[this->oper],
			type, op[0], op[1]);
		result = do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());
		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();
		break;
	}

	case ast_conditional:
	{
		/* From page 59 (page 65 of the PDF) of the GLSL 1.50 spec:
		*
		*    "The ternary selection operator (?:). It operates on three
		*    expressions (exp1 ? exp2 : exp3). This operator evaluates the
		*    first expression, which must result in a scalar Boolean."
		*/
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = this->subexpressions[1]->hir(instructions, state);
		op[2] = this->subexpressions[2]->hir(instructions, state);

		if (op[0]->type->base_type != GLSL_TYPE_BOOL)
		{
			YYLTYPE loc = this->get_location();
			const glsl_type* cond_type = glsl_type::get_instance(GLSL_TYPE_BOOL,
				op[0]->type->vector_elements, op[0]->type->matrix_columns);
			if (!apply_type_conversion(cond_type, op[0], instructions, state, false, &loc))
			{
				_mesa_glsl_error(&loc, state, "condition for ternary selection"
					"must evaluate to a scalar or vector boolean type");
				type = glsl_type::error_type;
				error_emitted = true;
			}
		}

		YYLTYPE loc = this->subexpressions[1]->get_location();
		if (!make_types_compatible(op[1], op[2], instructions, state, &loc, false))
		{
			_mesa_glsl_error(&loc, state, "Second and third operands of ?: "
				"operator must have matching types.");
			error_emitted = true;
			type = glsl_type::error_type;
		}
		else
		{
			type = op[1]->type;
		}

		if (!op[0]->type->is_scalar() &&
			(op[0]->type->vector_elements != type->vector_elements ||
			op[0]->type->matrix_columns != type->matrix_columns))
		{
			_mesa_glsl_error(&loc, state, "vector size of conditional does not match value");
			error_emitted = true;
			type = glsl_type::error_type;
		}

		if (type->is_error())
		{
			result = ir_rvalue::error_value(ctx);
		}
		else if (op[0]->type->is_scalar())
		{
			ir_constant *cond_val = op[0]->constant_expression_value();
			ir_constant *then_val = op[1]->constant_expression_value();
			ir_constant *else_val = op[2]->constant_expression_value();

			if ((cond_val != NULL) && (then_val != NULL) && (else_val != NULL))
			{
				result = (cond_val->value.b[0]) ? then_val : else_val;
			}
			else
			{
				ir_variable* tmp[3] = {0};
				for (unsigned i = 0; i < 3; i++)
				{
					tmp[i] = new(ctx)ir_variable(type, NULL, ir_var_temporary);
					instructions->push_tail(tmp[i]);
					if (i > 0)
					{
						instructions->push_tail(new(ctx)ir_assignment(
							new(ctx)ir_dereference_variable(tmp[i]),
							op[i]));
					}
				}

				ir_if *const stmt = new(ctx)ir_if(op[0]);
				stmt->then_instructions.push_tail(new(ctx)ir_assignment(
					new(ctx)ir_dereference_variable(tmp[0]),
					new(ctx)ir_dereference_variable(tmp[1])));
				stmt->else_instructions.push_tail(new(ctx)ir_assignment(
					new(ctx)ir_dereference_variable(tmp[0]),
					new(ctx)ir_dereference_variable(tmp[2])));
				instructions->push_tail(stmt);
				result = new(ctx)ir_dereference_variable(tmp[0]);
			}
		}
		else if(type->is_float())
		{
			int dim = type->vector_elements;
			ir_variable* tmp;
			tmp = new(ctx)ir_variable(type, NULL, ir_var_temporary);
			instructions->push_tail(tmp);
			for (int i = 0; i < dim; ++i)
			{
				ir_constant* const array_index = new (ctx) ir_constant(i);
				ir_dereference_array* array_bool = new(ctx)ir_dereference_array(op[0]->clone(ctx, nullptr), array_index);
				ir_dereference_array* array_out = new(ctx)ir_dereference_array(tmp, array_index);
				ir_dereference_array* array_1 = new(ctx)ir_dereference_array(op[1]->clone(ctx, nullptr), array_index);
				ir_dereference_array* array_2 = new(ctx)ir_dereference_array(op[2]->clone(ctx, nullptr), array_index);

				ir_if *const stmt = new(ctx)ir_if(array_bool);
				stmt->then_instructions.push_tail(new(ctx)ir_assignment(new(ctx)ir_dereference_array(tmp, array_index), array_1));
				stmt->else_instructions.push_tail(new(ctx)ir_assignment(new(ctx)ir_dereference_array(tmp, array_index), array_2));
				instructions->push_tail(stmt);
			}
			result = new(ctx)ir_dereference_variable(tmp);
		}
		else if (apply_type_conversion(type, op[0], instructions, state, false, &loc))
		{

			ir_variable* tmp[3] = { 0 };
			for (unsigned i = 0; i < 3; i++)
			{
				tmp[i] = new(ctx)ir_variable(type, NULL, ir_var_temporary);
				instructions->push_tail(tmp[i]);
				instructions->push_tail(new(ctx)ir_assignment(
					new(ctx)ir_dereference_variable(tmp[i]),
					op[i]));
			}


			if (type->is_integer())
			{
				ir_constant_data one_data = { 0 };
				for (unsigned i = 0; i < 16; ++i)
				{
					one_data.u[i] = 1;
				}

				ir_expression* expr_a = new(ctx)ir_expression(ir_binop_mul,
					new(ctx)ir_dereference_variable(tmp[0]),
					new(ctx)ir_dereference_variable(tmp[1]));
				ir_expression* expr_b = new(ctx)ir_expression(ir_binop_mul,
					new(ctx)ir_expression(ir_binop_sub,
						new(ctx)ir_constant(type, &one_data),
						new(ctx)ir_dereference_variable(tmp[0])),
					new(ctx)ir_dereference_variable(tmp[2]));
				result = new(ctx)ir_expression(ir_binop_add, type, expr_a, expr_b);
			}
			else
			{
				check(type->is_boolean());
				ir_expression* expr_a = new(ctx)ir_expression(ir_binop_logic_and,
					new(ctx)ir_dereference_variable(tmp[0]),
					new(ctx)ir_dereference_variable(tmp[1]));
				ir_expression* expr_b = new(ctx)ir_expression(ir_binop_logic_and,
					new(ctx)ir_expression(ir_unop_logic_not, new(ctx)ir_dereference_variable(tmp[0])),
					new(ctx)ir_dereference_variable(tmp[2]));
				result = new(ctx)ir_expression(ir_binop_logic_or, type, expr_a, expr_b);
			}
		}
		else
		{
			_mesa_glsl_error(&loc, state, "compiler error, cannot convert condition "
				"from '%s' to '%s' in ternary selection operator",
				op[0]->type->name, type->name);
			error_emitted = true;
			result = ir_rvalue::error_value(ctx);
			type = result->type;
		}
		break;
	}

	case ast_pre_inc:
	case ast_pre_dec:
	{
		this->non_lvalue_description = (this->oper == ast_pre_inc)
			? "pre-increment operation" : "pre-decrement operation";

		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = constant_one_for_inc_dec(ctx, op[0]->type);

		type = arithmetic_result_type(op[0], op[1], instructions, state, &loc, false);

		ir_rvalue *temp_rhs;
		temp_rhs = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);

		result = do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());
		error_emitted = op[0]->type->is_error();
		break;
	}

	case ast_post_inc:
	case ast_post_dec:
	{
		this->non_lvalue_description = (this->oper == ast_post_inc)
			? "post-increment operation" : "post-decrement operation";
		op[0] = this->subexpressions[0]->hir(instructions, state);
		op[1] = constant_one_for_inc_dec(ctx, op[0]->type);

		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();

		type = arithmetic_result_type(op[0], op[1], instructions, state, &loc, false);

		ir_rvalue *temp_rhs;
		temp_rhs = new(ctx)ir_expression(operations[this->oper], type,
			op[0], op[1]);

		/* Get a temporary of a copy of the lvalue before it's modified.
		* This may get thrown away later.
		*/
		result = get_lvalue_copy(instructions, op[0]->clone(ctx, NULL));

		(void)do_assignment(instructions, state,
			this->subexpressions[0]->non_lvalue_description,
			op[0]->clone(ctx, NULL), temp_rhs, false,
			this->subexpressions[0]->get_location());

		error_emitted = op[0]->type->is_error();
		break;
	}

	case ast_field_selection:
		result = _mesa_ast_field_selection_to_hir(this, instructions, state);
		break;

	case ast_array_index:
	{
		YYLTYPE index_loc = subexpressions[1]->get_location();

		op[0] = subexpressions[0]->hir(instructions, state);
		op[1] = subexpressions[1]->hir(instructions, state);

		error_emitted = op[0]->type->is_error() || op[1]->type->is_error();

		ir_rvalue *const array = op[0];

		bool const indexable_sampler = array->type->is_sampler() && (!array->type->sampler_ms) &&
			array->type->sampler_dimensionality < GLSL_SAMPLER_DIM_CUBE;

		if (indexable_sampler)
		{
			/** [] can index all textures, but the parameters need to change */
			ir_texture* texop = new(ctx)ir_texture(ir_txf, SSourceLocation());
			texop->sampler = op[0]->as_dereference();
			check(texop->sampler);

			uint32 dimensions = array->type->sampler_dimensionality + 1;
			dimensions += array->type->sampler_array;
			const glsl_type* index_type = glsl_type::get_instance(GLSL_TYPE_INT, dimensions, 1);

			if (op[1]->type->vector_elements != dimensions)
			{
				_mesa_glsl_error(&index_loc, state,
					"dimensions in sampler index incorrect");
				error_emitted = true;
			}

			texop->coordinate = op[1];
			apply_type_conversion(index_type, texop->coordinate, instructions, state, false, &loc);

			texop->type = glsl_type::vec4_type;
			if (array->type->sampler_buffer)
			{
				texop->lod_info.lod = NULL;
				if (state->LanguageSpec->UseSamplerInnerType() && array->type->inner_type)
				{
					texop->type = array->type->inner_type;
				}
			}
			else
			{
				texop->lod_info.lod = new(ctx)ir_constant(0);
			}

			result = texop;

			apply_type_conversion(array->type->inner_type, result, instructions, state, true, &loc);
		}
		else if (array->type->is_image())
		{
			/* the [] operator is being applied to an RW resource
			* need to inject special node into the ir to identify it
			*/
			if (!op[1]->type->is_error())
			{
				const glsl_type * index_type[] =
				{
					glsl_type::int_type,
					glsl_type::ivec2_type,
					glsl_type::ivec3_type,
					glsl_type::ivec4_type
				};

				//force the arguments to int, as GLSL requires the address arguments be ints
				apply_type_conversion(index_type[op[1]->type->vector_elements - 1], op[1], instructions, state, false, &loc);
			}

			result = new(ctx)ir_dereference_image(op[0], op[1]);
		}
		else if (array->type->is_patch())
		{
			result = new(ctx)ir_dereference_array(op[0], op[1]);
		}
		else
		{
			result = new(ctx)ir_dereference_array(op[0], op[1]);
		}

		/* Do not use op[0] after this point.  Use array.
		*/
		op[0] = NULL;


		if (error_emitted)
		{
			break;
		}

		if (!array->type->is_array()
			&& !array->type->is_matrix()
			&& !array->type->is_vector()
			&& !indexable_sampler
			&& !array->type->is_patch()
			&& !array->type->is_image())
		{
			_mesa_glsl_error(&index_loc, state,
				"cannot dereference non-array / non-matrix / "
				"non-vector");
			error_emitted = true;
		}

		if (!(op[1]->type->is_integer() || op[1]->type->is_boolean()))
		{
			_mesa_glsl_error(&index_loc, state,
				"array index must be integer type");
			error_emitted = true;
		}
		else if (array->type->is_image())
		{
			int32 required_count = -8;
			switch (array->type->sampler_dimensionality)
			{
			case GLSL_SAMPLER_DIM_1D:
			case GLSL_SAMPLER_DIM_BUF:
				required_count = 1;
				break;
			case GLSL_SAMPLER_DIM_2D:
				required_count = 2;
				break;
			case GLSL_SAMPLER_DIM_3D:
				required_count = 3;
				break;
			default:
				check(0);
				_mesa_glsl_error(&index_loc, state,
					"Unrecognized RW resource type");
				error_emitted = true;
			}
			required_count += array->type->sampler_array;
			if ((int32)op[1]->type->components() != required_count)
			{
				_mesa_glsl_error(&index_loc, state,
					"incorrect size for RW resource index (expected %d)", required_count);
				error_emitted = true;
			}
			else if (op[1]->type->is_matrix())
			{
				_mesa_glsl_error(&index_loc, state,
					"Attempt to index RW resource by matrix");
				error_emitted = true;
			}
		}
		else if (indexable_sampler)
		{
		}
		else if (!op[1]->type->is_scalar())
		{
			_mesa_glsl_error(&index_loc, state,
				"array index must be scalar");
			error_emitted = true;
		}

		/* If the array index is a constant expression and the array has a
		* declared size, ensure that the access is in-bounds.  If the array
		* index is not a constant expression, ensure that the array has a
		* declared size.
		*/
		ir_constant *const const_index = op[1]->constant_expression_value();
		if (array->type->is_sampler() ||
			array->type->is_image())
		{
			// no bound-checking possible
		}
		else if (const_index != NULL)
		{
			const int idx = const_index->value.i[0];
			const char *type_name;
			unsigned bound = 0;

			if (array->type->is_matrix())
			{
				type_name = "matrix";
			}
			else if (array->type->is_vector())
			{
				type_name = "vector";
			}
			else if (array->type->is_inputpatch())
			{
				type_name = "input patch";
			}
			else if (array->type->is_outputpatch())
			{
				type_name = "output patch";
			}
			else
			{
				check(array->type->is_array());
				type_name = "array";
			}

			/* From page 24 (page 30 of the PDF) of the GLSL 1.50 spec:
			*
			*    "It is illegal to declare an array with a size, and then
			*    later (in the same shader) index the same array with an
			*    integral constant expression greater than or equal to the
			*    declared size. It is also illegal to index an array with a
			*    negative constant expression."
			*/
			if (array->type->is_matrix())
			{
				if (array->type->row_type()->vector_elements <= idx)
				{
					bound = array->type->row_type()->vector_elements;
				}
			}
			else if (array->type->is_vector())
			{
				if (array->type->vector_elements <= idx)
				{
					bound = array->type->vector_elements;
				}
			}
			else if (array->type->is_patch())
			{
				if ((array->type->patch_size() > 0)
					&& (array->type->patch_size() <= idx))
				{
					bound = array->type->patch_size();
				}
			}
			else
			{
				if ((array->type->array_size() > 0)
					&& (array->type->array_size() <= idx))
				{
					bound = array->type->array_size();
				}
			}

			if (bound > 0)
			{
				_mesa_glsl_error(&loc, state, "%s index must be < %u",
					type_name, bound);
				error_emitted = true;
			}
			else if (idx < 0)
			{
				_mesa_glsl_error(&loc, state, "%s index must be >= 0",
					type_name);
				error_emitted = true;
			}

			if (array->type->is_array())
			{
				/* If the array is a variable dereference, it dereferences the
				* whole array, by definition.  Use this to get the variable.
				*
				* FINISHME: Should some methods for getting / setting / testing
				* FINISHME: array access limits be added to ir_dereference?
				*/
				ir_variable *const v = array->whole_variable_referenced();
				if ((v != NULL) && (unsigned(idx) > v->max_array_access))
				{
					v->max_array_access = idx;
				}
			}
		}
		else if (array->type->array_size() == 0)
		{
			_mesa_glsl_error(&loc, state, "unsized array index must be constant");
		}
		else
		{
			if (op[1]->type->is_boolean())
			{
				apply_type_conversion(glsl_type::get_instance(GLSL_TYPE_INT, 1, 1), op[1], instructions, state, false, &loc);
			}
			if (array->type->is_array())
			{
				/* whole_variable_referenced can return NULL if the array is a
				* member of a structure.  In this case it is safe to not update
				* the max_array_access field because it is never used for fields
				* of structures.
				*/
				ir_variable *v = array->whole_variable_referenced();
				if (v != NULL)
				{
					v->max_array_access = array->type->array_size() - 1;
				}
			}

			/* FINISHME: handle patches*/
		}

		/* From page 23 (29 of the PDF) of the GLSL 1.30 spec:
		*
		*    "Samplers aggregated into arrays within a shader (using square
		*    brackets [ ]) can only be indexed with integral constant
		*    expressions [...]."
		*
		* This restriction was added in GLSL 1.30.  Shaders using earlier version
		* of the language should not be rejected by the compiler front-end for
		* using this construct.  This allows useful things such as using a loop
		* counter as the index to an array of samplers.  If the loop in unrolled,
		* the code should compile correctly.  Instead, emit a warning.
		*/
		if (array->type->is_array() &&
			array->type->element_type()->is_sampler() &&
			const_index == NULL)
		{
			if (state->language_version == 100)
			{
				_mesa_glsl_warning(&loc, state,
					"sampler arrays indexed with non-constant "
					"expressions is optional in GLSL ES 1.00");
			}
			else if (state->language_version < 130)
			{
				_mesa_glsl_warning(&loc, state,
					"sampler arrays indexed with non-constant "
					"expressions is forbidden in GLSL 1.30 and "
					"later");
			}
			else
			{
				_mesa_glsl_error(&loc, state,
					"sampler arrays indexed with non-constant "
					"expressions is forbidden in GLSL 1.30 and "
					"later");
				error_emitted = true;
			}
		}

		if (error_emitted)
		{
			result->type = glsl_type::error_type;
		}

		break;
	}

	case ast_function_call:
		/* Should *NEVER* get here.  ast_function_call should always be handled
		* by ast_function_expression::hir.
		*/
		check(0);
		break;

	case ast_initializer_list:
		// Should never get here.
		check(0);
		break;

	case ast_identifier:
	{
		/* ast_identifier can appear several places in a full abstract syntax
		* tree.  This particular use must be at location specified in the grammar
		* as 'variable_identifier'.
		*/
		ir_variable *var = state->symbols->get_variable(this->primary_expression.identifier);

		if (var != NULL)
		{
			var->used = true;
			result = new(ctx)ir_dereference_variable(var);
		}
		else
		{
			_mesa_glsl_error(&loc, state, "'%s' undeclared", this->primary_expression.identifier);

			result = ir_rvalue::error_value(ctx);
			error_emitted = true;
		}
		break;
	}

	case ast_int_constant:
		result = new(ctx)ir_constant(this->primary_expression.int_constant);
		break;

	case ast_uint_constant:
		result = new(ctx)ir_constant(this->primary_expression.uint_constant);
		break;

	case ast_float_constant:
		result = new(ctx)ir_constant(this->primary_expression.float_constant);
		break;

	case ast_bool_constant:
		result = new(ctx)ir_constant(bool(this->primary_expression.bool_constant));
		break;

	case ast_sequence:
	{
		/* It should not be possible to generate a sequence in the AST without
		* any expressions in it.
		*/
		check(!this->expressions.is_empty());

		/* The r-value of a sequence is the last expression in the sequence.  If
		* the other expressions in the sequence do not have side-effects (and
		* therefore add instructions to the instruction list), they get dropped
		* on the floor.
		*/
		exec_node *previous_tail_pred = NULL;
		YYLTYPE previous_operand_loc = loc;

		foreach_list_typed(ast_node, ast, link, &this->expressions)
		{
			/* If one of the operands of comma operator does not generate any
			* code, we want to emit a warning.  At each pass through the loop
			* previous_tail_pred will point to the last instruction in the
			* stream *before* processing the previous operand.  Naturally,
			* instructions->tail_pred will point to the last instruction in the
			* stream *after* processing the previous operand.  If the two
			* pointers match, then the previous operand had no effect.
			*
			* The warning behavior here differs slightly from GCC.  GCC will
			* only emit a warning if none of the left-hand operands have an
			* effect.  However, it will emit a warning for each.  I believe that
			* there are some cases in C (especially with GCC extensions) where
			* it is useful to have an intermediate step in a sequence have no
			* effect, but I don't think these cases exist in GLSL.  Either way,
			* it would be a giant hassle to replicate that behavior.
			*/
			if (previous_tail_pred == instructions->tail_pred)
			{
				_mesa_glsl_warning(&previous_operand_loc, state,
					"left-hand operand of comma expression has "
					"no effect");
			}

			/* tail_pred is directly accessed instead of using the get_tail()
			* method for performance reasons.  get_tail() has extra code to
			* return NULL when the list is empty.  We don't care about that
			* here, so using tail_pred directly is fine.
			*/
			previous_tail_pred = instructions->tail_pred;
			previous_operand_loc = ast->get_location();

			result = ast->hir(instructions, state);
		}

		/* Any errors should have already been emitted in the loop above.
		*/
		error_emitted = true;
		break;
	}

	case ast_type_cast:
	{
		const char *type_name;
		const glsl_type *to_type;

		check(primary_expression.type_specifier);
		to_type = primary_expression.type_specifier->glsl_type(&type_name, state);
		if (to_type)
		{
			result = subexpressions[0]->hir(instructions, state);
			error_emitted = result->type->is_error();
			if (apply_type_conversion(to_type, result, instructions, state, true, &loc) == false)
			{
				_mesa_glsl_error(&loc, state, "cannot convert from '%s' to '%s'",
					result->type->name, to_type->name);
				result = ir_rvalue::error_value(ctx);
				error_emitted = true;
			}
		}
		else
		{
			_mesa_glsl_error(&loc, state, "unknown type '%s' in type cast operator",
				primary_expression.type_specifier->type_name);
			result = ir_rvalue::error_value(ctx);
			error_emitted = true;
		}
	}
	}
	type = NULL; /* use result->type, not type. */

	if (result || this->oper != ast_field_selection)
	{
		check(result != NULL);

		if (result->type->is_error() && !error_emitted)
		{
			_mesa_glsl_error(&loc, state, "type mismatch");
		}
	}

	return result;
}


ir_rvalue* ast_expression_statement::hir(exec_list *instructions, _mesa_glsl_parse_state *state)
{
	/* It is possible to have expression statements that don't have an
	* expression.  This is the solitary semicolon:
	*
	* for (i = 0; i < 5; i++)
	*     ;
	*
	* In this case the expression will be NULL.  Test for NULL and don't do
	* anything in that case.
	*/
	if (expression != NULL)
	{
		expression->hir(instructions, state);
	}

	/* Statements do not have r-values.
	*/
	return NULL;
}


ir_rvalue* ast_compound_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	if (new_scope)
	{
		state->symbols->push_scope();
	}

	foreach_list_typed(ast_node, ast, link, &this->statements)
	{
		ast->hir(instructions, state);
	}

	if (new_scope)
	{
		state->symbols->pop_scope();
	}

	/* Compound statements do not have r-values.
	*/
	return NULL;
}


static const glsl_type * process_array_type(YYLTYPE *loc, const glsl_type *base, ast_node *array_size, struct _mesa_glsl_parse_state *state)
{
	unsigned length = 0;

	while (array_size != NULL)
	{
		exec_list dummy_instructions;
		ir_rvalue *const ir = array_size->hir(&dummy_instructions, state);
		YYLTYPE loc = array_size->get_location();

		if (ir != NULL)
		{
			if (!ir->type->is_integer())
			{
				_mesa_glsl_error(&loc, state, "array size must be integer type");
			}
			else if (!ir->type->is_scalar())
			{
				_mesa_glsl_error(&loc, state, "array size must be scalar type");
			}
			else
			{
				ir_constant *const size = ir->constant_expression_value();

				if (size == NULL)
				{
					_mesa_glsl_error(&loc, state, "array size must be a "
						"constant valued expression");
				}
				else if (size->value.i[0] <= 0)
				{
					_mesa_glsl_error(&loc, state, "array size must be > 0");
				}
				else
				{
					check(size->type == ir->type);
					length = size->value.u[0];

					/* If the array size is const (and we've verified that
					* it is) then no instructions should have been emitted
					* when we converted it to HIR.  If they were emitted,
					* then either the array size isn't const after all, or
					* we are emitting unnecessary instructions.
					*/
					check(dummy_instructions.is_empty());
				}
			}
		}

		base = glsl_type::get_array_instance(base, length);
		exec_node* next_node = array_size->link.get_next();
		array_size = next_node ? exec_node_data(ast_node, next_node, link) : NULL;
	}

	if (length == 0)
	{
		return glsl_type::get_array_instance(base, 0);
	}

	return base;
}


const glsl_type* ast_type_specifier::glsl_type(const char **name, _mesa_glsl_parse_state *state) const
{
	const struct glsl_type *type = nullptr;

	YYLTYPE loc = this->get_location();

	if (IsStructuredOrRWStructuredBuffer())
	{
		const bool bRWStructuredBuffer = !strcmp(this->type_name + 2, "StructuredBuffer");
		const struct glsl_type* InnerType = nullptr;
		if (this->InnerStructure)
		{
			InnerType = state->symbols->get_type(this->InnerStructure->name);
		}
		else
		{
			InnerType = state->symbols->get_type(this->inner_type);
		}

		// Emulate structured buffer with a typed buffer if platform does not properly support them. Only for vec4&vec1 atm
		// Android devices with MALI GPUs do not support SSBO in vertex shaders (OpenGL)
		if (state->LanguageSpec->EmulateStructuredWithTypedBuffers())
		{
			if ((InnerType == glsl_type::vec4_type || InnerType == glsl_type::float_type) ||
				(InnerType == glsl_type::ivec4_type || InnerType == glsl_type::int_type) ||
				(InnerType == glsl_type::uvec4_type || InnerType == glsl_type::uint_type))
			{
				const char* emulated_type_name = bRWStructuredBuffer ? "RWBuffer" : "Buffer";
				type = glsl_type::get_templated_instance(InnerType, emulated_type_name, this->texture_ms_num_samples, this->patch_size);
				check(type != NULL);
				*name = emulated_type_name;
			}
		}
		else
		{
			type = glsl_type::GetStructuredBufferInstance(this->type_name, InnerType);
			*name = type->name;
		}
	}
	else if (!strcmp(this->type_name, "ByteAddressBuffer") || !strcmp(this->type_name + 2, "ByteAddressBuffer"))
	{
		type = glsl_type::GetByteAddressBufferInstance(this->type_name);
		*name = type->name;
	}
	else if (this->inner_type)
	{
		// Lazily create sampler or outputstream types with specified return types.
		const struct glsl_type* inner_type = NULL;
		inner_type = state->symbols->get_type(this->inner_type);
		type = glsl_type::get_templated_instance(inner_type, this->type_name,
			this->texture_ms_num_samples,
			this->patch_size
			);
		check(type != NULL);
		*name = type->name;
	}
	else
	{
		type = state->symbols->get_type(this->type_name);
		*name = this->type_name;
	}

	if (this->is_array)
	{
		type = process_array_type(&loc, type, this->array_size, state);
	}

	return type;
}


static void apply_type_qualifier_to_variable(const struct ast_type_qualifier *qual,
	ir_variable *var, struct _mesa_glsl_parse_state *state, YYLTYPE *loc)
{
	if (qual->flags.q.invariant)
	{
		if (var->used)
		{
			_mesa_glsl_error(loc, state,
				"variable '%s' may not be redeclared "
				"'invariant' after being used",
				var->name);
		}
		else
		{
			var->invariant = 1;
		}
	}

	// handle image resources special
	// image resource is a uniform variable, but are allowed to write to it
	if (var->type->base_type == GLSL_TYPE_IMAGE)
	{
		if (qual->flags.q.constant || qual->flags.q.attribute
			|| (qual->flags.q.varying && (state->target == fragment_shader)))
		{
			var->read_only = 1;
		}

		if (var->type->HlslName && (!strncmp(var->type->HlslName, "RWStructuredBuffer<", 19) 
			|| !strncmp(var->type->HlslName, "StructuredBuffer<", 17)))
		{
			var->location = state->uavLocation++;
			var->explicit_location = true;
		}
	}
	else
	{
		if (qual->flags.q.constant || qual->flags.q.attribute
			|| qual->flags.q.uniform
			|| (qual->flags.q.varying && (state->target == fragment_shader)))
		{
			var->read_only = 1;
		}
	}

	if (qual->flags.q.centroid)
	{
		var->centroid = 1;
	}

	if (qual->flags.q.gs_point)
	{
		var->geometryinput = ir_geometry_input_points;
	}
	else if (qual->flags.q.gs_line)
	{
		var->geometryinput = ir_geometry_input_lines;
	}
	else if (qual->flags.q.gs_lineadj)
	{
		var->geometryinput = ir_geometry_input_lines_adjacent;
	}
	else if (qual->flags.q.gs_triangle)
	{
		var->geometryinput = ir_geometry_input_triangles;
	}
	else if (qual->flags.q.gs_triangleadj)
	{
		var->geometryinput = ir_geometry_input_triangles_adjacent;
	}
	else
	{
		var->geometryinput = ir_geometry_input_none;
	}

	if (qual->flags.q.coherent)
	{
		if (var->type->base_type != GLSL_TYPE_IMAGE)
		{
			_mesa_glsl_error(loc, state, " 'globallycoherent' "
				"qualifier only valid on RW resources");
		}
		else
		{
			var->coherent = 1;
		}
	}

	if (qual->flags.q.attribute && state->target != vertex_shader)
	{
		var->type = glsl_type::error_type;
		_mesa_glsl_error(loc, state,
			"'attribute' variables may not be declared in the "
			"%s shader",
			_mesa_glsl_shader_target_name(state->target));
	}

	/* From page 25 (page 31 of the PDF) of the GLSL 1.10 spec:
	*
	*     "The varying qualifier can be used only with the data types
	*     float, vec2, vec3, vec4, mat2, mat3, and mat4, or arrays of
	*     these."
	*/
	if (qual->flags.q.varying)
	{
		const glsl_type *non_array_type;

		if (var->type && var->type->is_array())
		{
			non_array_type = var->type->fields.array;
		}
		else
			non_array_type = var->type;
		{

		}
		if (non_array_type && !non_array_type->is_float())
		{
			var->type = glsl_type::error_type;
			_mesa_glsl_error(loc, state,
				"varying variables must be of base type float");
		}
	}

	/* If there is no qualifier that changes the mode of the variable, leave
	* the setting alone.
	*/
	if (qual->flags.q.in && qual->flags.q.out)
	{
		var->mode = ir_var_inout;
	}
	else if (qual->flags.q.attribute || qual->flags.q.in
		|| (qual->flags.q.varying && (state->target == fragment_shader)))
	{
		var->mode = ir_var_in;
	}
	else if (qual->flags.q.out
		|| (qual->flags.q.varying && (state->target == vertex_shader)))
	{
		var->mode = ir_var_out;
	}
	else if (qual->flags.q.shared)
	{
		var->mode = ir_var_shared;
	}
	else if (qual->flags.q.uniform)
	{
		var->mode = ir_var_uniform;
	}

	if (state->all_invariant && (state->current_function == NULL))
	{
		switch (state->target)
		{
		case vertex_shader:
			if (var->mode == ir_var_out)
			{
				var->invariant = true;
			}
			break;
		case geometry_shader:
			if ((var->mode == ir_var_in) || (var->mode == ir_var_out))
			{
				var->invariant = true;
			}
			break;
		case fragment_shader:
			if (var->mode == ir_var_in)
			{
				var->invariant = true;
			}
			break;
		default:
			break;
		}
	}

	if (qual->flags.q.flat)
	{
		var->interpolation = ir_interp_qualifier_flat;
	}
	else if (qual->flags.q.noperspective)
	{
		var->interpolation = ir_interp_qualifier_noperspective;
	}
	else if (qual->flags.q.smooth)
	{
		var->interpolation = ir_interp_qualifier_smooth;
	}
	else
	{
		var->interpolation = ir_interp_qualifier_none;
	}

	// This error-check is commented-out, because one of those qualifiers may be present on a variable that will
	// actually not be used in the shader, and will be optimized-out later.
	/*
	if (var->interpolation != ir_interp_qualifier_none &&
	!(state->target == vertex_shader && var->mode == ir_var_out) &&
	!(state->target == fragment_shader && var->mode == ir_var_in)) {
	const char *qual_string = NULL;
	switch (var->interpolation) {
	case ir_interp_qualifier_flat:
	qual_string = "flat";
	break;
	case ir_interp_qualifier_noperspective:
	qual_string = "noperspective";
	break;
	case ir_interp_qualifier_smooth:
	qual_string = "smooth";
	break;
	}

	_mesa_glsl_error(loc, state,
	"interpolation qualifier '%s' can only be applied to "
	"vertex and geometry shader outputs and fragment shader inputs.",
	qual_string);

	}
	*/

	var->pixel_center_integer = qual->flags.q.pixel_center_integer;
	var->origin_upper_left = qual->flags.q.origin_upper_left;
	if ((qual->flags.q.origin_upper_left || qual->flags.q.pixel_center_integer)
		&& (strcmp(var->name, "gl_FragCoord") != 0))
	{
		const char *const qual_string = (qual->flags.q.origin_upper_left)
			? "origin_upper_left" : "pixel_center_integer";

		_mesa_glsl_error(loc, state,
			"layout qualifier '%s' can only be applied to "
			"fragment shader input 'gl_FragCoord'",
			qual_string);
	}

	if (qual->flags.q.explicit_location)
	{
		const bool global_scope = (state->current_function == NULL);
		bool fail = false;
		const char *string = "";

		/* In the vertex shader only shader inputs can be given explicit
		* locations.
		*
		* In the fragment shader only shader outputs can be given explicit
		* locations.
		*/
		switch (state->target)
		{
		case vertex_shader:
			if (!global_scope || (var->mode != ir_var_in))
			{
				fail = true;
				string = "input";
			}
			break;

		case geometry_shader:
			_mesa_glsl_error(loc, state,
				"geometry shader variables cannot be given "
				"explicit locations\n");
			break;

		case fragment_shader:
			if (!global_scope || (var->mode != ir_var_out))
			{
				fail = true;
				string = "output";
			}
			break;
		default:
			break;
		};

		if (fail)
		{
			_mesa_glsl_error(loc, state,
				"only %s shader %s variables can be given an "
				"explicit location\n",
				_mesa_glsl_shader_target_name(state->target),
				string);
		}
		else
		{
			var->explicit_location = true;
			var->location = qual->location;
		}
	}

	/* Does the declaration use the 'layout' keyword?
	*/
	const bool uses_layout = qual->flags.q.pixel_center_integer
		|| qual->flags.q.origin_upper_left
		|| qual->flags.q.explicit_location;

	/* Does the declaration use the deprecated 'attribute' or 'varying'
	* keywords?
	*/
	const bool uses_deprecated_qualifier = qual->flags.q.attribute
		|| qual->flags.q.varying;

	/* Layout qualifiers for gl_FragDepth, which are enabled by extension
	* AMD_conservative_depth.
	*/
	if (qual->flags.q.depth_any)
	{
		var->depth_layout = ir_depth_layout_any;
	}
	else if (qual->flags.q.depth_greater)
	{
		var->depth_layout = ir_depth_layout_greater;
	}
	else if (qual->flags.q.depth_less)
	{
		var->depth_layout = ir_depth_layout_less;
	}
	else if (qual->flags.q.depth_unchanged)
	{
		var->depth_layout = ir_depth_layout_unchanged;
	}
	else
	{
		var->depth_layout = ir_depth_layout_none;
	}
}

/**
* Get the variable that is being redeclared by this declaration
*
* Semantic checks to verify the validity of the redeclaration are also
* performed.  If semantic checks fail, compilation error will be emitted via
* \c _mesa_glsl_error, but a non-\c NULL pointer will still be returned.
*
* \returns
* A pointer to an existing variable in the current scope if the declaration
* is a redeclaration, \c NULL otherwise.
*/
ir_variable * get_variable_being_redeclared(ir_variable *var, ast_declaration *decl, struct _mesa_glsl_parse_state *state)
{
	/* Check if this declaration is actually a re-declaration, either to
	* resize an array or add qualifiers to an existing variable.
	*
	* This is allowed for variables in the current scope, or when at
	* global scope (for built-ins in the implicit outer scope).
	*/
	ir_variable *earlier = state->symbols->get_variable(decl->identifier);
	if (earlier == NULL ||
		(state->current_function != NULL &&
		!state->symbols->name_declared_this_scope(decl->identifier)))
	{
		return NULL;
	}


	YYLTYPE loc = decl->get_location();

	/* From page 24 (page 30 of the PDF) of the GLSL 1.50 spec,
	*
	* "It is legal to declare an array without a size and then
	*  later re-declare the same name as an array of the same
	*  type and specify a size."
	*/
	if ((earlier->type->array_size() == 0)
		&& var->type->is_array()
		&& (var->type->element_type() == earlier->type->element_type()))
	{
		/* FINISHME: This doesn't match the qualifiers on the two
		* FINISHME: declarations.  It's not 100% clear whether this is
		* FINISHME: required or not.
		*/

		const unsigned size = unsigned(var->type->array_size());
		if ((size > 0) && (size <= earlier->max_array_access))
		{
			_mesa_glsl_error(&loc, state, "array size must be > %u due to "
				"previous access",
				earlier->max_array_access);
		}

		earlier->type = var->type;
		delete var;
		var = NULL;
	}
	else
	{
		_mesa_glsl_error(&loc, state, "'%s' redeclared", decl->identifier);
	}

	return earlier;
}

/**
* Generate the IR for an initializer in a variable declaration
*/
ir_rvalue * process_initializer(ir_variable *var, ast_declaration *decl,
	ast_fully_specified_type *type, exec_list *initializer_instructions, struct _mesa_glsl_parse_state *state)
{
	ir_rvalue *result = NULL;

	YYLTYPE initializer_loc = decl->initializer->get_location();

	/* From page 24 (page 30 of the PDF) of the GLSL 1.10 spec:
	*
	*    "All uniform variables are read-only and are initialized either
	*    directly by an application via API commands, or indirectly by
	*    OpenGL."
	*/
	if (var->mode == ir_var_uniform)
	{
		_mesa_glsl_warning(&initializer_loc, state,
			"uniform initializer will be ignored");
		return NULL;
	}

	if ((var->mode == ir_var_in) && (state->current_function == NULL))
	{
		_mesa_glsl_error(&initializer_loc, state,
			"cannot initialize %s shader input / %s",
			_mesa_glsl_shader_target_name(state->target),
			(state->target == vertex_shader)
			? "attribute" : "varying");
	}

	ir_dereference *const lhs = new(state)ir_dereference_variable(var);
	ir_rvalue *rhs = decl->initializer->initializer_hir(
		initializer_instructions, state, lhs->type);

	/* Calculate the constant value if this is a const or uniform
	* declaration.
	*/
	if (type->qualifier.flags.q.constant || type->qualifier.flags.q.uniform)
	{
		ir_rvalue *new_rhs = validate_assignment(state, initializer_instructions, var->type, rhs, true, &initializer_loc);
		if (new_rhs != NULL)
		{
			rhs = new_rhs;

			ir_constant *constant_value = rhs->constant_expression_value();
			if (!constant_value)
			{
				// Since we assumed all statics are const, we do allow one assignment
				/*
				if (type->qualifier.flags.q.is_static || type->qualifier.flags.q.uniform)
				{
				_mesa_glsl_error(& initializer_loc, state,
				"initializer of %s variable '%s' must be a "
				"constant expression",
				(type->qualifier.flags.q.constant)
				? "const" : "uniform",
				decl->identifier);
				if (var->type->is_numeric())
				{
				/ * Reduce cascading errors. * /
				var->constant_value = ir_constant::zero(state, var->type);
				}
				}
				*/
			}
			else
			{
				rhs = constant_value;
				var->constant_value = constant_value;
			}
		}
		else
		{
			_mesa_glsl_error(&initializer_loc, state,
				"initializer of type %s cannot be assigned to "
				"variable of type %s",
				rhs->type->name, var->type->name);
			if (var->type->is_numeric())
			{
				/* Reduce cascading errors. */
				var->constant_value = ir_constant::zero(state, var->type);
			}
		}
	}

	if (rhs && !rhs->type->is_error())
	{
		bool temp = var->read_only;
		if (type->qualifier.flags.q.constant)
		{
			var->read_only = false;
		}

		/* Never emit code to initialize a uniform.
		*/
		const glsl_type *initializer_type;
		if (!type->qualifier.flags.q.uniform)
		{
			result = do_assignment(initializer_instructions, state,
				NULL,
				lhs, rhs, true,
				type->get_location());
			initializer_type = result->type;
		}
		else
		{
			initializer_type = rhs->type;
		}

		var->constant_initializer = rhs->constant_expression_value();
		var->has_initializer = true;

		/* If the declared variable is an unsized array, it must inherrit
		* its full type from the initializer.  A declaration such as
		*
		*     uniform float a[] = float[](1.0, 2.0, 3.0, 3.0);
		*
		* becomes
		*
		*     uniform float a[4] = float[](1.0, 2.0, 3.0, 3.0);
		*
		* The assignment generated in the if-statement (below) will also
		* automatically handle this case for non-uniforms.
		*
		* If the declared variable is not an array, the types must
		* already match exactly.  As a result, the type assignment
		* here can be done unconditionally.  For non-uniforms the call
		* to do_assignment can change the type of the initializer (via
		* the implicit conversion rules).  For uniforms the initializer
		* must be a constant expression, and the type of that expression
		* was validated above.
		*/
		var->type = initializer_type;

		var->read_only = temp;
	}

	return result;
}

ir_rvalue* ast_declarator_list::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;
	const struct glsl_type *decl_type;
	const char *type_name = NULL;
	ir_rvalue *result = NULL;
	YYLTYPE loc = this->get_location();

	/* From page 46 (page 52 of the PDF) of the GLSL 1.50 spec:
	*
	*     "To ensure that a particular output variable is invariant, it is
	*     necessary to use the invariant qualifier. It can either be used to
	*     qualify a previously declared variable as being invariant
	*
	*         invariant gl_Position; // make existing gl_Position be invariant"
	*
	* In these cases the parser will set the 'invariant' flag in the declarator
	* list, and the type will be NULL.
	*/
	if (this->invariant)
	{
		check(this->type == NULL);

		if (state->current_function != NULL)
		{
			_mesa_glsl_error(&loc, state,
				"All uses of 'invariant' keyword must be at global "
				"scope\n");
		}

		foreach_list_typed(ast_declaration, decl, link, &this->declarations)
		{
			check(!decl->is_array);
			check(decl->array_size == NULL);
			check(decl->initializer == NULL);

			ir_variable *const earlier =
				state->symbols->get_variable(decl->identifier);
			if (earlier == NULL)
			{
				_mesa_glsl_error(&loc, state,
					"Undeclared variable '%s' cannot be marked "
					"invariant\n", decl->identifier);
			}
            else if ((state->target == vertex_shader)
                && (earlier->mode != ir_var_out))
            {
				if (!state->LanguageSpec->AllowsInvariantBufferTypes() || !earlier->type->sampler_buffer)
				{
					_mesa_glsl_error(&loc, state,
						"'%s' cannot be marked invariant, vertex shader "
						"outputs only\n", decl->identifier);
				}
            }
            else if ((state->target == fragment_shader)
                && (earlier->mode != ir_var_in))
            {
				if (!state->LanguageSpec->AllowsInvariantBufferTypes() || !earlier->type->sampler_buffer)
				{
					_mesa_glsl_error(&loc, state,
						"'%s' cannot be marked invariant, fragment shader "
						"inputs only\n", decl->identifier);
				}
            }
			else if (earlier->used)
			{
				_mesa_glsl_error(&loc, state,
					"variable '%s' may not be redeclared "
					"'invariant' after being used",
					earlier->name);
			}
			else
			{
				earlier->invariant = true;
			}
		}

		/* Invariant redeclarations do not have r-values.
		*/
		return NULL;
	}

	check(this->type != NULL);
	check(!this->invariant);

	/* The type specifier may contain a structure definition.  Process that
	* before any of the variable declarations.
	*/
	(void) this->type->specifier->hir(instructions, state);

	decl_type = this->type->specifier->glsl_type(&type_name, state);
	if (this->declarations.is_empty())
	{
		/* If there is no structure involved in the program text, there are two
		* possible scenarios:
		*
		* - The program text contained something like 'vec4;'.  This is an
		*   empty declaration.  It is valid but weird.  Emit a warning.
		*
		* - The program text contained something like 'S;' and 'S' is not the
		*   name of a known structure type.  This is both invalid and weird.
		*   Emit an error.
		*
		* Note that if decl_type is NULL and there is a structure involved,
		* there must have been some sort of error with the structure.  In this
		* case we assume that an error was already generated on this line of
		* code for the structure.  There is no need to generate an additional,
		* confusing error.
		*/
		check(this->type->specifier->structure == NULL || decl_type != NULL
			|| state->error);
		if (this->type->specifier->structure == NULL)
		{
			if (decl_type != NULL)
			{
				_mesa_glsl_warning(&loc, state, "empty declaration");
			}
			else
			{
				_mesa_glsl_error(&loc, state,
					"invalid type '%s' in empty declaration",
					type_name);
			}
		}
	}

	// Handle row/column major qualifiers for matrices.
	if (decl_type && decl_type->is_matrix())
	{
		// If the matrix was declared without a layout qualifer, it is row_major.
		if (this->type->qualifier.flags.q.row_major == 0
			&& this->type->qualifier.flags.q.column_major == 0)
		{
			this->type->qualifier.flags.q.row_major = 1;
		}

		// The parser doesn't allow both row_major and column_major.
		if (this->type->qualifier.flags.q.row_major == 1
			&& this->type->qualifier.flags.q.column_major == 1)
		{
			_mesa_glsl_error(&loc, state, "a matrix cannot be both row and column major");
			this->type->qualifier.flags.q.column_major = 0;
		}

		// Only row-major types are supported!
		if (this->type->qualifier.flags.q.column_major == 1)
		{
			_mesa_glsl_error(&loc, state, "column_major matrices are not supported");
		}
	}

	foreach_list_typed(ast_declaration, decl, link, &this->declarations)
	{
		const struct glsl_type *var_type;
		ir_variable *var;

		if (decl_type == nullptr && type->specifier->IsStructuredOrRWStructuredBuffer())
		{
			// Ignore for now
			continue;
		}

		/* FINISHME: Emit a warning if a variable declaration shadows a
		* FINISHME: declaration at a higher scope.
		*/

		if ((decl_type == NULL) || decl_type->is_void())
		{
			if (type_name != NULL)
			{
				_mesa_glsl_error(&loc, state,
					"invalid type '%s' in declaration of '%s'",
					type_name, decl->identifier);
			}
			else
			{
				_mesa_glsl_error(&loc, state,
					"invalid type in declaration of '%s'",
					decl->identifier);
			}
			continue;
		}

		if (decl->is_array)
		{
			var_type = process_array_type(&loc, decl_type, decl->array_size,
				state);
			if (var_type->is_error())
				continue;
		}
		else
		{
			var_type = decl_type;
		}

		var = new(ctx)ir_variable(var_type, decl->identifier, ir_var_auto);

		apply_type_qualifier_to_variable(&this->type->qualifier, var, state,
			&loc);

		if (this->type->qualifier.flags.q.invariant)
		{
            if ((state->target == vertex_shader) && !(var->mode == ir_var_out ||
                var->mode == ir_var_inout))
            {
				/* FINISHME: Note that this doesn't work for invariant on
				 * a function signature outval
				 */
				if (!state->LanguageSpec->AllowsInvariantBufferTypes() || !var->type->sampler_buffer)
				{
					_mesa_glsl_error(&loc, state,
									 "'%s' cannot be marked invariant, vertex shader "
									 "outputs only\n", var->name);
				}
            }
            else if ((state->target == fragment_shader) &&
                !(var->mode == ir_var_in || var->mode == ir_var_inout))
            {
                /* FINISHME: Note that this doesn't work for invariant on
                * a function signature inval
                */
				if (!state->LanguageSpec->AllowsInvariantBufferTypes() || !var->type->sampler_buffer)
				{
					_mesa_glsl_error(&loc, state,
									 "'%s' cannot be marked invariant, fragment shader "
									 "inputs only\n", var->name);
				}
            }
		}

		if (state->current_function != NULL)
		{
			const char *mode = NULL;
			const char *extra = "";

			/* There is no need to check for 'inout' here because the parser will
			* only allow that in function parameter lists.
			*/
			if (this->type->qualifier.flags.q.attribute)
			{
				mode = "attribute";
			}
			else if (this->type->qualifier.flags.q.uniform)
			{
				mode = "uniform";
			}
			else if (this->type->qualifier.flags.q.varying)
			{
				mode = "varying";
			}
			else if (this->type->qualifier.flags.q.in)
			{
				mode = "in";
				extra = " or in function parameter list";
			}
			else if (this->type->qualifier.flags.q.out)
			{
				mode = "out";
				extra = " or in function parameter list";
			}

			if (mode)
			{
				_mesa_glsl_error(&loc, state,
					"%s variable '%s' must be declared at "
					"global scope%s",
					mode, var->name, extra);
			}
		}
		else if (var->mode == ir_var_in)
		{
			var->read_only = true;

			if (state->target == vertex_shader)
			{
				bool error_emitted = false;

				/* From page 31 (page 37 of the PDF) of the GLSL 1.50 spec:
				*
				*    "Vertex shader inputs can only be float, floating-point
				*    vectors, matrices, signed and unsigned integers and integer
				*    vectors. Vertex shader inputs can also form arrays of these
				*    types, but not structures."
				*
				* From page 31 (page 27 of the PDF) of the GLSL 1.30 spec:
				*
				*    "Vertex shader inputs can only be float, floating-point
				*    vectors, matrices, signed and unsigned integers and integer
				*    vectors. They cannot be arrays or structures."
				*
				* From page 23 (page 29 of the PDF) of the GLSL 1.20 spec:
				*
				*    "The attribute qualifier can be used only with float,
				*    floating-point vectors, and matrices. Attribute variables
				*    cannot be declared as arrays or structures."
				*/
				const glsl_type *check_type = var->type->is_array()
					? var->type->fields.array : var->type;

				switch (check_type->base_type)
				{
				case GLSL_TYPE_HALF:
				case GLSL_TYPE_FLOAT:
					break;
				case GLSL_TYPE_UINT:
				case GLSL_TYPE_INT:
					if (state->language_version > 120)
						break;
					/* FALLTHROUGH */
				default:
					_mesa_glsl_error(&loc, state,
						"vertex shader input / attribute cannot have "
						"type %s'%s'",
						var->type->is_array() ? "array of " : "",
						check_type->name);
					error_emitted = true;
				}

				if (!error_emitted && (state->language_version <= 130)
					&& var->type->is_array())
				{
					_mesa_glsl_error(&loc, state,
						"vertex shader input / attribute cannot have "
						"array type");
					error_emitted = true;
				}
			}
		}

		/* Integer vertex outputs must be qualified with 'flat'.
		*
		* From section 4.3.6 of the GLSL 1.30 spec:
		*    "If a vertex output is a signed or unsigned integer or integer
		*    vector, then it must be qualified with the interpolation qualifier
		*    flat."
		*/
		if (state->language_version >= 130
			&& state->target == vertex_shader
			&& state->current_function == NULL
			&& var->type->is_integer()
			&& var->mode == ir_var_out
			&& var->interpolation != ir_interp_qualifier_flat)
		{

			_mesa_glsl_error(&loc, state, "If a vertex output is an integer, "
				"then it must be qualified with 'flat'");
		}


		/* Interpolation qualifiers cannot be applied to 'centroid' and
		* 'centroid varying'.
		*
		* From page 29 (page 35 of the PDF) of the GLSL 1.30 spec:
		*    "interpolation qualifiers may only precede the qualifiers in,
		*    centroid in, out, or centroid out in a declaration. They do not apply
		*    to the deprecated storage qualifiers varying or centroid varying."
		*/
		if (state->language_version >= 130
			&& this->type->qualifier.has_interpolation()
			&& this->type->qualifier.flags.q.varying)
		{

			const char *i = this->type->qualifier.interpolation_string();
			check(i != NULL);
			const char *s;
			if (this->type->qualifier.flags.q.centroid)
				s = "centroid varying";
			else
				s = "varying";

			_mesa_glsl_error(&loc, state,
				"qualifier '%s' cannot be applied to the "
				"deprecated storage qualifier '%s'", i, s);
		}


		/* Interpolation qualifiers can only apply to vertex shader outputs and
		* fragment shader inputs.
		*
		* From page 29 (page 35 of the PDF) of the GLSL 1.30 spec:
		*    "Outputs from a vertex shader (out) and inputs to a fragment
		*    shader (in) can be further qualified with one or more of these
		*    interpolation qualifiers"
		*/
		if (state->language_version >= 130
			&& this->type->qualifier.has_interpolation())
		{

			const char *i = this->type->qualifier.interpolation_string();
			check(i != NULL);

			switch (state->target)
			{
			case vertex_shader:
				if (this->type->qualifier.flags.q.in)
				{
					_mesa_glsl_error(&loc, state,
						"qualifier '%s' cannot be applied to vertex "
						"shader inputs", i);
				}
				break;
			case fragment_shader:
				if (this->type->qualifier.flags.q.out)
				{
					_mesa_glsl_error(&loc, state,
						"qualifier '%s' cannot be applied to fragment "
						"shader outputs", i);
				}
				break;
			case geometry_shader:
				break;
			default:
				check(0);
			}
		}


		/* From section 4.3.4 of the GLSL 1.30 spec:
		*    "It is an error to use centroid in in a vertex shader."
		*/
		if (state->language_version >= 130
			&& this->type->qualifier.flags.q.centroid
			&& this->type->qualifier.flags.q.in
			&& state->target == vertex_shader)
		{

			_mesa_glsl_error(&loc, state,
				"'centroid in' cannot be used in a vertex shader");
		}


		/* Precision qualifiers exists only in GLSL versions 1.00 and >= 1.30.
		*/
		if (this->type->specifier->precision != ast_precision_none
			&& state->language_version != 100
			&& state->language_version < 130)
		{

			_mesa_glsl_error(&loc, state,
				"precision qualifiers are supported only in GLSL ES "
				"1.00, and GLSL 1.30 and later");
		}


		/* Precision qualifiers only apply to floating point and integer types.
		*
		* From section 4.5.2 of the GLSL 1.30 spec:
		*    "Any floating point or any integer declaration can have the type
		*    preceded by one of these precision qualifiers [...] Literal
		*    constants do not have precision qualifiers. Neither do Boolean
		*    variables.
		*
		* In GLSL ES, sampler types are also allowed.
		*
		* From page 87 of the GLSL ES spec:
		*    "RESOLUTION: Allow sampler types to take a precision qualifier."
		*/
		/*
		if (this->type->specifier->precision != ast_precision_none
		&& !var->type->is_float()
		&& !var->type->is_integer()
		&& !(var->type->is_sampler())
		&& !(var->type->is_array()
		&& (var->type->fields.array->is_float()
		|| var->type->fields.array->is_integer()))) {

		_mesa_glsl_error(&loc, state,
		"precision qualifiers apply only to floating point"
		"%s types", state->es_shader ? ", integer, and sampler"
		: " and integer");
		}*/

		/* From page 17 (page 23 of the PDF) of the GLSL 1.20 spec:
		*
		*    "[Sampler types] can only be declared as function
		*    parameters or uniform variables (see Section 4.3.5
		*    "Uniform")".
		*/
		if (var_type->contains_sampler() && (state->current_function == NULL) && !this->type->qualifier.flags.q.uniform)
		{
			if (this->type->qualifier.flags.q.is_static && !decl->initializer)
			{
				_mesa_glsl_error(&loc, state, "static sampler has a missing initializer");
			}
			else if (!this->type->qualifier.flags.q.is_static)
			{
				_mesa_glsl_error(&loc, state, "samplers must be declared uniform");
			}
		}

/*
		// We only allow static const declarations, even though HLSL does allow to change the value of a static;
		// this simplifies constant value propagation
		if (this->type->qualifier.flags.q.is_static && !this->type->qualifier.flags.q.constant)
		{
			_mesa_glsl_error(&loc, state, "static variables must be declared const");
		}
*/

		/* Process the initializer and add its instructions to a temporary
		* list.  This list will be added to the instruction stream (below) after
		* the declaration is added.  This is done because in some cases (such as
		* redeclarations) the declaration may not actually be added to the
		* instruction stream.
		*/
		exec_list initializer_instructions;
		ir_variable *earlier = get_variable_being_redeclared(var, decl, state);

		// Make static non-const variables initialized to zero as FXC does
		if (this->type->qualifier.flags.q.is_static && !this->type->qualifier.flags.q.constant && !this->type->specifier->structure && !decl->initializer)
		{
			ast_expression* Zero = new(ctx)ast_expression(ast_int_constant, NULL, NULL, NULL);
			Zero->set_location(decl->get_location());
			Zero->primary_expression.int_constant = 0;

			ast_expression* Cast = new(ctx)ast_expression(ast_type_cast, Zero, NULL, NULL);
			Cast->set_location(decl->get_location());
			Cast->primary_expression.type_specifier = this->type->specifier;

			decl->initializer = new(ctx)ast_initializer_list_expression();
			Cast->link.self_link();
			decl->initializer->expressions.push_degenerate_list_at_head(&Cast->link);
		}

		if (decl->initializer != NULL)
		{
			result = process_initializer((earlier == NULL) ? var : earlier,
				decl, this->type,
				&initializer_instructions, state);
		}

		/* From page 23 (page 29 of the PDF) of the GLSL 1.10 spec:
		*
		*     "It is an error to write to a const variable outside of
		*      its declaration, so they must be initialized when
		*      declared."
		*/
		if (this->type->qualifier.flags.q.constant && decl->initializer == NULL)
		{
			_mesa_glsl_error(&loc, state,
				"const declaration of '%s' must be initialized",
				decl->identifier);
		}

		/* If the declaration is not a redeclaration, there are a few additional
		* semantic checks that must be applied.  In addition, variable that was
		* created for the declaration should be added to the IR stream.
		*/
		if (earlier == NULL)
		{
			/* From page 15 (page 21 of the PDF) of the GLSL 1.10 spec,
			*
			*   "Identifiers starting with "gl_" are reserved for use by
			*   OpenGL, and may not be declared in a shader as either a
			*   variable or a function."
			*/
			if (strncmp(decl->identifier, "gl_", 3) == 0)
				_mesa_glsl_error(&loc, state,
				"identifier '%s' uses reserved 'gl_' prefix",
				decl->identifier);
			else if (strstr(decl->identifier, "__"))
			{
				/* From page 14 (page 20 of the PDF) of the GLSL 1.10
				* spec:
				*
				*     "In addition, all identifiers containing two
				*      consecutive underscores (__) are reserved as
				*      possible future keywords."
				*/
				_mesa_glsl_error(&loc, state,
					"identifier '%s' uses reserved '__' string",
					decl->identifier);
			}

			/* Add the variable to the symbol table.  Note that the initializer's
			* IR was already processed earlier (though it hasn't been emitted
			* yet), without the variable in scope.
			*
			* This differs from most C-like languages, but it follows the GLSL
			* specification.  From page 28 (page 34 of the PDF) of the GLSL 1.50
			* spec:
			*
			*     "Within a declaration, the scope of a name starts immediately
			*     after the initializer if present or immediately after the name
			*     being declared if not."
			*/
			if (!state->symbols->add_variable(var))
			{
				YYLTYPE loc = this->get_location();
				_mesa_glsl_error(&loc, state, "name '%s' already taken in the "
					"current scope", decl->identifier);
				continue;
			}

			/* Push the variable declaration to the top.  It means that all the
			* variable declarations will appear in a funny last-to-first order,
			* but otherwise we run into trouble if a function is prototyped, a
			* global var is decled, then the function is defined with usage of
			* the global var.  See glslparsertest's CorrectModule.frag.
			*/
			instructions->push_head(var);
		}

		instructions->append_list(&initializer_instructions);
		//var->initializer_instructions.append_list(&initializer_instructions);
	}


	/* Generally, variable declarations do not have r-values.  However,
	* one is used for the declaration in
	*
	* while (bool b = some_condition()) {
	*   ...
	* }
	*
	* so we return the rvalue from the last seen declaration here.
	*/
	return result;
}


ir_rvalue * ast_parameter_declarator::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;
	const struct glsl_type *type;
	const char *name = NULL;
	YYLTYPE loc = this->get_location();

	type = this->type->specifier->glsl_type(&name, state);

	if (type == NULL)
	{
		if (name != NULL)
		{
			_mesa_glsl_error(&loc, state,
				"invalid type '%s' in declaration of '%s'",
				name, this->identifier);
		}
		else
		{
			_mesa_glsl_error(&loc, state,
				"invalid type in declaration of '%s'",
				this->identifier);
		}

		type = glsl_type::error_type;
	}

	/* From page 62 (page 68 of the PDF) of the GLSL 1.50 spec:
	*
	*    "Functions that accept no input arguments need not use void in the
	*    argument list because prototypes (or definitions) are required and
	*    therefore there is no ambiguity when an empty argument list "( )" is
	*    declared. The idiom "(void)" as a parameter list is provided for
	*    convenience."
	*
	* Placing this check here prevents a void parameter being set up
	* for a function, which avoids tripping up checks for main taking
	* parameters and lookups of an unnamed symbol.
	*/
	if (type->is_void())
	{
		if (this->identifier != NULL)
			_mesa_glsl_error(&loc, state,
			"named parameter cannot have type 'void'");

		is_void = true;
		return NULL;
	}

	if (formal_parameter && (this->identifier == NULL))
	{
		_mesa_glsl_error(&loc, state, "formal parameter lacks a name");
		return NULL;
	}

	/* This only handles "vec4 foo[..]".  The earlier specifier->glsl_type(...)
	* call already handled the "vec4[..] foo" case.
	*/
	if (this->is_array)
	{
		type = process_array_type(&loc, type, this->array_size, state);
	}

	if (!type->is_error() && type->array_size() == 0)
	{
		_mesa_glsl_error(&loc, state, "arrays passed as parameters must have "
			"a declared size.");
		type = glsl_type::error_type;
	}

	is_void = false;
	ir_variable *var = new(ctx)ir_variable(type, this->identifier, ir_var_in);

	/* HLSL: Store the semantic to which the parameter is bound. */
	var->semantic = ralloc_strdup(var, semantic);

	/* Apply any specified qualifiers to the parameter declaration.  Note that
	* for function parameters the default mode is 'in'.
	*/
	apply_type_qualifier_to_variable(&this->type->qualifier, var, state, &loc);

	/* From page 17 (page 23 of the PDF) of the GLSL 1.20 spec:
	*
	*    "Samplers cannot be treated as l-values; hence cannot be used
	*    as out or inout function parameters, nor can they be assigned
	*    into."
	*/
	if ((var->mode == ir_var_inout || var->mode == ir_var_out)
		&& type->contains_sampler())
	{
		_mesa_glsl_error(&loc, state, "out and inout parameters cannot contain samplers");
		type = glsl_type::error_type;
	}

	/* From page 39 (page 45 of the PDF) of the GLSL 1.10 spec:
	*
	*    "When calling a function, expressions that do not evaluate to
	*     l-values cannot be passed to parameters declared as out or inout."
	*
	* From page 32 (page 38 of the PDF) of the GLSL 1.10 spec:
	*
	*    "Other binary or unary expressions, non-dereferenced arrays,
	*     function names, swizzles with repeated fields, and constants
	*     cannot be l-values."
	*
	* So for GLSL 1.10, passing an array as an out or inout parameter is not
	* allowed.  This restriction is removed in GLSL 1.20, and in GLSL ES.
	*/
	if ((var->mode == ir_var_inout || var->mode == ir_var_out)
		&& type->is_array() && state->language_version == 110)
	{
		_mesa_glsl_error(&loc, state, "Arrays cannot be out or inout parameters in GLSL 1.10");
		type = glsl_type::error_type;
	}

	instructions->push_tail(var);

	/* Parameter declarations do not have r-values.
	*/
	return NULL;
}


void ast_parameter_declarator::parameters_to_hir(exec_list *ast_parameters, bool formal, exec_list *ir_parameters,
	unsigned *num_params, exec_list *ir_defaults, unsigned *num_defaults, _mesa_glsl_parse_state *state)
{
	ast_parameter_declarator *void_param = NULL;
	unsigned count = 0;
	unsigned default_count = 0;

	foreach_list_typed(ast_parameter_declarator, param, link, ast_parameters)
	{
		param->formal_parameter = formal;
		param->hir(ir_parameters, state);

		if (param->is_void)
			void_param = param;

		if (param->default_value)
		{
			exec_list temp_ir;
			ir_rvalue* default_value = param->default_value->hir(&temp_ir, state);
			ir_constant* const_value = default_value->constant_expression_value();
			if (const_value == NULL)
			{
				YYLTYPE loc = param->get_location();
				_mesa_glsl_error(&loc, state, "default value for function "
					"parameter '%s' must be a constant expression",
					param->identifier);
			}
			else if (param->type->qualifier.flags.q.out)
			{
				YYLTYPE loc = param->get_location();
				_mesa_glsl_error(&loc, state, "output parameter '%s' may not have "
					"a default parameters", param->identifier);
			}
			else
			{
				ir_defaults->push_tail(const_value);
				default_count++;
			}
		}
		else if (default_count > 0)
		{
			YYLTYPE loc = param->get_location();
			_mesa_glsl_error(&loc, state, "function parameter '%s' must have a "
				"default parameter", param->identifier);
		}

		count++;
	}

	*num_params = count;
	*num_defaults = default_count;

	if ((void_param != NULL) && (count > 1))
	{
		YYLTYPE loc = void_param->get_location();

		_mesa_glsl_error(&loc, state,
			"'void' parameter must be only parameter");
	}
}


void emit_function(_mesa_glsl_parse_state *state, ir_function *f)
{
	/* IR invariants disallow function declarations or definitions
	* nested within other function definitions.  But there is no
	* requirement about the relative order of function declarations
	* and definitions with respect to one another.  So simply insert
	* the new ir_function block at the end of the toplevel instruction
	* list.
	*/
	state->toplevel_ir->push_tail(f);
}


ir_rvalue * ast_function::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;
	ir_function *f = NULL;
	ir_function_signature *sig = NULL;
	exec_list hir_parameters;
	exec_list parameter_defaults;
	unsigned num_params = 0;
	unsigned num_defaults = 0;

	const char *const name = identifier;

	/* New functions are always added to the top-level IR instruction stream,
	* so this instruction list pointer is ignored.  See also emit_function
	* (called below).
	*/
	(void)instructions;

	/* From page 21 (page 27 of the PDF) of the GLSL 1.20 spec,
	*
	*   "Function declarations (prototypes) cannot occur inside of functions;
	*   they must be at global scope, or for the built-in functions, outside
	*   the global scope."
	*
	* From page 27 (page 33 of the PDF) of the GLSL ES 1.00.16 spec,
	*
	*   "User defined functions may only be defined within the global scope."
	*
	* Note that this language does not appear in GLSL 1.10.
	*/
	if ((state->current_function != NULL) && (state->language_version != 110))
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state,
			"declaration of function '%s' not allowed within "
			"function body", name);
	}

	/* From page 15 (page 21 of the PDF) of the GLSL 1.10 spec,
	*
	*   "Identifiers starting with "gl_" are reserved for use by
	*   OpenGL, and may not be declared in a shader as either a
	*   variable or a function."
	*/
	if (strncmp(name, "gl_", 3) == 0)
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state,
			"identifier '%s' uses reserved 'gl_' prefix", name);
	}

	/* Convert the list of function parameters to HIR now so that they can be
	* used below to compare this function's signature with previously seen
	* signatures for functions with the same name.
	*/
	ast_parameter_declarator::parameters_to_hir(&this->parameters,
		is_definition, &hir_parameters, &num_params,
		&parameter_defaults, &num_defaults, state);

	bool has_out_parameters = false;
	foreach_iter(exec_list_iterator, iter, hir_parameters)
	{
		ir_variable* var = (ir_variable*)iter.get();
		if (var->mode == ir_var_out || var->mode == ir_var_inout)
		{
			has_out_parameters = true;
			break;
		}
	}

	const char *return_type_name;
	const glsl_type *return_type =
		this->return_type->specifier->glsl_type(&return_type_name, state);

	if (!return_type)
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state,
			"function '%s' has undeclared return type '%s'",
			name, return_type_name);
		return_type = glsl_type::error_type;
	}

	/* From page 56 (page 62 of the PDF) of the GLSL 1.30 spec:
	* "No qualifier is allowed on the return type of a function."
	*/
	if (this->return_type->has_qualifiers())
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state,
			"function '%s' return type has qualifiers", name);
	}

	/* From page 17 (page 23 of the PDF) of the GLSL 1.20 spec:
	*
	*    "[Sampler types] can only be declared as function parameters
	*    or uniform variables (see Section 4.3.5 "Uniform")".
	*/
	if (return_type->contains_sampler())
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state,
			"function '%s' return type can't contain a sampler",
			name);
	}

	/* Verify that this function's signature either doesn't match a previously
	* seen signature for a function with the same name, or, if a match is found,
	* that the previously seen signature does not have an associated definition.
	*/
	f = state->symbols->get_function(name);
	if (f != NULL)
	{
		sig = f->exact_matching_signature(&hir_parameters);
		if (sig != NULL)
		{
			const char *badvar = sig->qualifiers_match(&hir_parameters);
			if (badvar != NULL)
			{
				YYLTYPE loc = this->get_location();

				_mesa_glsl_error(&loc, state, "function '%s' parameter '%s' "
					"qualifiers don't match prototype", name, badvar);
			}

			if (sig->return_type != return_type)
			{
				YYLTYPE loc = this->get_location();

				_mesa_glsl_error(&loc, state, "function '%s' return type doesn't "
					"match prototype", name);
			}

			if ((!return_semantic && sig->return_semantic) ||
				(return_semantic && !sig->return_semantic) ||
				(return_semantic && sig->return_semantic && stricmp(sig->return_semantic, return_semantic) != 0))
			{
				YYLTYPE loc = this->get_location();
				_mesa_glsl_error(&loc, state, "function '%s' return semantic doesn't "
					"match prototype", name);
			}

			if (is_definition && sig->is_defined)
			{
				YYLTYPE loc = this->get_location();

				_mesa_glsl_error(&loc, state, "function '%s' redefined", name);
			}
		}
	}
	else
	{
		f = new(ctx)ir_function(name);
		if (!state->symbols->add_function(f))
		{
			/* This function name shadows a non-function use of the same name. */
			YYLTYPE loc = this->get_location();

			_mesa_glsl_error(&loc, state, "function name '%s' conflicts with "
				"non-function", name);
			return NULL;
		}

		emit_function(state, f);
	}

	/* Finish storing the information about this new function in its signature.
	*/
	if (sig == NULL)
	{
		sig = new(ctx)ir_function_signature(return_type);
		if (return_semantic)
		{
			sig->return_semantic = ralloc_strdup(sig, return_semantic);
		}
		f->add_signature(sig);
	}

	sig->replace_parameters(&hir_parameters);
	sig->has_output_parameters = has_out_parameters;
	signature = sig;

	/* Generate signatures to handle the case of default values. */
	if (f && num_defaults > 0)
	{
		YYLTYPE loc = this->get_location();
		unsigned min_params = num_params - num_defaults;
		for (unsigned i = min_params; i < num_params; ++i)
		{
			ir_function_signature* default_sig = new(ctx)ir_function_signature(return_type);
			ir_variable* return_var = NULL;
			if (!return_type->is_void())
			{
				return_var = new(ctx)ir_variable(return_type, NULL, ir_var_temporary);
				default_sig->body.push_tail(return_var);
			}

			exec_list actual_parameters;
			ir_variable* param = (ir_variable*)sig->parameters.get_head();
			ir_constant* default_value = (ir_constant*)parameter_defaults.get_head();
			for (unsigned p = 0; p < i; ++p)
			{
				ir_variable* var = param->clone(ctx, NULL);
				default_sig->parameters.push_tail(var);
				actual_parameters.push_tail(new(ctx)ir_dereference_variable(var));

				param = (ir_variable*)param->get_next();
				if (p >= min_params)
				{
					default_value = (ir_constant*)default_value->get_next();
				}
			}

			for (unsigned p = i; p < num_params; ++p)
			{
				ir_rvalue* param_value = default_value->clone(ctx, NULL);
				if (!apply_type_conversion(param->type, param_value, &default_sig->body, state, false, &loc))
				{
					_mesa_glsl_error(&loc, state, "default value for parameter "
						"'%s' does not match type '%s'",
						param->name, param->type->name);
				}
				actual_parameters.push_tail(param_value);

				param = (ir_variable*)param->get_next();
				default_value = (ir_constant*)default_value->get_next();
			}

			if (return_var)
			{
				default_sig->body.push_tail(new(ctx)ir_call(
					sig,
					new(ctx)ir_dereference_variable(return_var),
					&actual_parameters));
				default_sig->body.push_tail(new(ctx)ir_return(
					new(ctx)ir_dereference_variable(return_var)));
			}
			else
			{
				default_sig->body.push_tail(new(ctx)ir_call(sig, NULL, &actual_parameters));
				default_sig->body.push_tail(new(ctx)ir_return());
			}

			default_sig->is_defined = true;
			default_sig->has_output_parameters = has_out_parameters;
			f->add_signature(default_sig);
		}
	}

	/* Function declarations (prototypes) do not have r-values.
	*/
	return NULL;
}

// static type inference so we can special case retrieval of string attribute arguments

template<typename T> struct attrib_arg_converter
{
	static bool convert_arg(const char** argument, ast_attribute_arg * arg, bool (glsl_type::*type_check)(void) const, struct _mesa_glsl_parse_state * state)
	{
		return false;
	}

	template<typename Y>
	static bool convert_arg(Y* argument, ast_attribute_arg * arg, bool (glsl_type::*type_check)(void) const, struct _mesa_glsl_parse_state * state)
	{
		if (!arg->is_string)
		{
			exec_list dummy_list; /* don't want attributes appending instructions to the real hir */
			ir_rvalue *arg_ir = arg->argument.exp_argument->hir(&dummy_list, state);

			if (arg_ir && (arg_ir->type->*type_check)() && arg_ir->type->is_scalar() && arg_ir->constant_expression_value())
			{
				T result;
				arg_ir->constant_expression_value()->get_component(result, 0);
				*argument = result;
				return true;
			}
		}
		return false;
	}
};

template<> struct attrib_arg_converter<const char*>
{
	static bool convert_arg(const char** argument, ast_attribute_arg * arg, bool (glsl_type::*type_check)(void) const, struct _mesa_glsl_parse_state * state)
	{
		if (arg->is_string)
		{
			// duplicate and skip the leading and trailing ""
			const char* result = ralloc_strndup(state, arg->argument.string_argument + 1, strlen(arg->argument.string_argument) - 2);

			*argument = result;
			return true;
		}
		return false;
	}
	template<typename T>
	static bool convert_arg(T* argument, ast_attribute_arg * arg, bool (glsl_type::*type_check)(void) const, struct _mesa_glsl_parse_state * state)
	{
		return false;
	}
};

template<typename Type>
bool ast_function_definition::convert_attribute_args(
	ast_attribute * attrib,
	Type* results,
	const int expected_arg_count,
	const char* expected_type,
	bool (glsl_type::*type_check)(void) const,
struct _mesa_glsl_parse_state * state
	) const
{
	int arg_count = 0;
	// iterate the arg list
	foreach_list_const(m, &attrib->arguments)
	{
		arg_count++;

		if (arg_count > expected_arg_count)
			break;

		ast_attribute_arg *arg = exec_node_data(ast_attribute_arg, m, link);

		if (!attrib_arg_converter<Type>::convert_arg(results + (arg_count - 1), arg, type_check, state))
		{
			struct YYLTYPE loc = get_location();
			_mesa_glsl_error(&loc, state, "invalid argument to attribute '%s'%s%s%s: expected constant expression of type '%s'%s",
				attrib->attribute_name,
				arg->is_string ? " '" : "",
				arg->is_string ? arg->argument.string_argument : "",
				arg->is_string ? "' " : "",
				expected_type,
				arg->is_string ? ", but found a 'string'" : ""
				);
		}
	}

	if (arg_count != expected_arg_count)
	{
		struct YYLTYPE loc = get_location();
		_mesa_glsl_error(&loc, state, "invalid number of argument(s) to '%s' attribute, expected %d arguments of type '%s', but got %d instead", attrib->attribute_name, expected_arg_count, expected_type, arg_count);
		return false;
	}
	return true;
}

static const char* domain_strings[GLSL_DOMAIN_ISOLINE] =
{
	"tri",
	"quad",
	"isoline",
};

static const glsl_domain domain_values[GLSL_DOMAIN_ISOLINE] =
{
	GLSL_DOMAIN_TRI,
	GLSL_DOMAIN_QUAD,
	GLSL_DOMAIN_ISOLINE
};
static const char* partitioning_strings[GLSL_PARTITIONING_POW2] =
{
	"integer",
	"fractional_even",
	"fractional_odd",
	"pow2",
};

static const glsl_partitioning partitioning_values[GLSL_PARTITIONING_POW2] =
{
	GLSL_PARTITIONING_INTEGER,
	GLSL_PARTITIONING_FRACTIONAL_EVEN,
	GLSL_PARTITIONING_FRACTIONAL_ODD,
	GLSL_PARTITIONING_POW2,
};
static const char* outputtopology_strings[GLSL_OUTPUTTOPOLOGY_TRIANGLE_CCW] =
{
	"point",
	"line",
	"triangle_cw",
	"triangle_ccw",
};

static const glsl_outputtopology outputtopology_values[GLSL_OUTPUTTOPOLOGY_TRIANGLE_CCW] =
{
	GLSL_OUTPUTTOPOLOGY_POINT,
	GLSL_OUTPUTTOPOLOGY_LINE,
	GLSL_OUTPUTTOPOLOGY_TRIANGLE_CW,
	GLSL_OUTPUTTOPOLOGY_TRIANGLE_CCW
};

template<typename T>
T convert_string_to_enum(const T default_value, const char* string, const char** strings, const T* values, const size_t num_values)
{
	for (int i = 0; i < num_values; ++i)
	{
		if (!strcmp(strings[i], string))
		{
			return values[i];
		}
	}

	return default_value;
}

template<typename T>
void ast_function_definition::convert_enum_attribute_args(ast_attribute * attrib, T& result, const char** strings, const T* values, const size_t num_values, struct _mesa_glsl_parse_state * state)  const
{
	const char* result_string = 0;

	if (convert_attribute_args(attrib, &result_string, 1, "string", 0, state))
	{
		result = convert_string_to_enum(T(0), result_string, strings, values, num_values);
	}

	if (result == T(0))
	{
		struct YYLTYPE loc = get_location();
		_mesa_glsl_error(&loc, state, "invalid value '%s' for '%s' attribute ", result_string, attrib->attribute_name);
	}
}


ir_rvalue* ast_function_definition::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	prototype->is_definition = true;
	prototype->hir(instructions, state);

	ir_function_signature *signature = prototype->signature;
	if (signature == NULL)
	{
		return NULL;
	}

	check(state->current_function == NULL);
	state->current_function = signature;
	state->found_return = false;

	/* Duplicate parameters declared in the prototype as concrete variables.
	* Add these to the symbol table.
	*/
	state->symbols->push_scope();
	foreach_iter(exec_list_iterator, iter, signature->parameters)
	{
		ir_variable *const var = ((ir_instruction *)iter.get())->as_variable();

		check(var != NULL);

		/* The only way a parameter would "exist" is if two parameters have
		* the same name.
		*/
		if (state->symbols->name_declared_this_scope(var->name))
		{
			YYLTYPE loc = this->get_location();

			_mesa_glsl_error(&loc, state, "parameter '%s' redeclared", var->name);
		}
		else
		{
			state->symbols->add_variable(var);
		}
	}

	/*
	* Attribute parsing for function signatures
	* Parses attributes and tags information into the hir
	* The only errors thrown here are for ill-formed attributes
	* Invalid attributes are not errors, must catch unsupported
	* conditions when writing out, because unused functions
	* shouldn't cause a failure.
	**/

	/* Parse attributes attached to this function. */
	foreach_list_const(n, &this->attributes)
	{
		ast_attribute *attrib = exec_node_data(ast_attribute, n, link);

		/* ToDo: Do attributes need to be case insensitive? */

		/* Check for earlydepthstencil attached for pixel shaders */
		if (!strcmp(attrib->attribute_name, "earlydepthstencil"))
		{
			signature->is_early_depth_stencil = 1;
		}
		/* check for numthreads for compute shaders */
		else if (!strcmp(attrib->attribute_name, "numthreads"))
		{
			int results[3] = {0, 0, 0};

			convert_attribute_args(attrib, results, 3, "int", &glsl_type::is_integer, state);

			signature->wg_size_x = results[0];
			signature->wg_size_y = results[1];
			signature->wg_size_z = results[2];

		}
		else if (!strcmp(attrib->attribute_name, "maxvertexcount"))
		{
			int result = 0;

			convert_attribute_args(attrib, &result, 1, "int", &glsl_type::is_integer, state);

			signature->maxvertexcount = result;
		}
		else if (!strcmp(attrib->attribute_name, "maxtessfactor"))
		{
			float result = 0.0f;

			convert_attribute_args(attrib, &result, 1, "float", &glsl_type::is_numeric, state);

			signature->tessellation.maxtessfactor = result;

		}
		else if (!strcmp(attrib->attribute_name, "outputcontrolpoints"))
		{
			int result = 0.0f;

			convert_attribute_args(attrib, &result, 1, "int", &glsl_type::is_integer, state);

			signature->tessellation.outputcontrolpoints = result;

		}
		else if (!strcmp(attrib->attribute_name, "patchconstantfunc"))
		{
			const char* result = 0;

			convert_attribute_args(attrib, &result, 1, "string", 0, state);

			signature->tessellation.patchconstantfunc = result;

		}
		else if (!strcmp(attrib->attribute_name, "domain"))
		{
			glsl_domain result = GLSL_DOMAIN_NONE;

			convert_enum_attribute_args(attrib, result, domain_strings, domain_values, GetNumArrayElements(domain_values), state);

			signature->tessellation.domain = result;

		}
		else if (!strcmp(attrib->attribute_name, "partitioning"))
		{
			glsl_partitioning result = GLSL_PARTITIONING_NONE;

			convert_enum_attribute_args(attrib, result, partitioning_strings, partitioning_values, GetNumArrayElements(partitioning_values), state);

			signature->tessellation.partitioning = result;

		}
		else if (!strcmp(attrib->attribute_name, "outputtopology"))
		{
			glsl_outputtopology result = GLSL_OUTPUTTOPOLOGY_NONE;

			convert_enum_attribute_args(attrib, result, outputtopology_strings, outputtopology_values, GetNumArrayElements(outputtopology_values), state);

			signature->tessellation.outputtopology = result;

		}
		else
		{
			/* unrecognized attributes, could warn */
			YYLTYPE loc = this->get_location();
			_mesa_glsl_warning(&loc, state, "unrecognized attribute %s", attrib->attribute_name);

		}
	}

	/* Convert the body of the function to HIR. */
	this->body->hir(&signature->body, state);
	signature->is_defined = true;

	state->symbols->pop_scope();

	check(state->current_function == signature);
	state->current_function = NULL;

	if (!signature->return_type->is_void() && !state->found_return)
	{
		YYLTYPE loc = this->get_location();
		_mesa_glsl_error(&loc, state, "function '%s' has non-void return type "
			"%s, but no return statement",
			signature->function_name(),
			signature->return_type->name);
	}

	/* Function definitions do not have r-values.
	*/
	return NULL;
}


ir_rvalue* ast_jump_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	switch (mode)
	{
	case ast_return:
	{
		ir_return *inst;
		check(state->current_function);

		if (opt_return_value)
		{
			ir_rvalue * ret = opt_return_value->hir(instructions, state);

			/* The value of the return type can be NULL if the shader says
			* 'return foo();' and foo() is a function that returns void.
			*
			* NOTE: The GLSL spec doesn't say that this is an error.  The type
			* of the return value is void.  If the return type of the function is
			* also void, then this should compile without error.  Seriously.
			*/
			const glsl_type *const ret_type =
				(ret == NULL) ? glsl_type::void_type : ret->type;

			YYLTYPE loc = this->get_location();
			if ((ret_type != state->current_function->return_type &&
				ret == NULL) ||
				!apply_type_conversion(state->current_function->return_type,
				ret, instructions, state, false, &loc))
			{
				_mesa_glsl_error(&loc, state,
					"'return' with wrong type %s, in function '%s' "
					"returning %s",
					ret_type->name,
					state->current_function->function_name(),
					state->current_function->return_type->name);
			}

			inst = new(ctx)ir_return(ret);
		}
		else
		{
			if (state->current_function->return_type->base_type != GLSL_TYPE_VOID)
			{
				YYLTYPE loc = this->get_location();

				_mesa_glsl_error(&loc, state,
					"'return' with no value, in function %s returning "
					"non-void",
					state->current_function->function_name());
			}
			inst = new(ctx)ir_return;
		}

		state->found_return = true;
		instructions->push_tail(inst);
		break;
	}

	case ast_discard:
/*
		if (state->target != fragment_shader)
		{
			YYLTYPE loc = this->get_location();
			_mesa_glsl_error(&loc, state, "'discard' may only appear in a fragment shader");
		}
*/
		instructions->push_tail(new(ctx)ir_discard);
		break;

	case ast_break:
	case ast_continue:
		if (mode == ast_continue &&
			state->loop_nesting_ast == NULL)
		{
			YYLTYPE loc = this->get_location();

			_mesa_glsl_error(&loc, state,
				"continue may only appear in a loop");
		}
		else if (mode == ast_break &&
			state->loop_nesting_ast == NULL &&
			state->switch_state.switch_nesting_ast == NULL)
		{
			YYLTYPE loc = this->get_location();

			_mesa_glsl_error(&loc, state,
				"break may only appear in a loop or a switch");
		}
		else
		{
			/* For a loop, inline the for loop expression again,
			* since we don't know where near the end of
			* the loop body the normal copy of it
			* is going to be placed.
			*/
			if (state->loop_nesting_ast != NULL &&
				mode == ast_continue &&
				state->loop_nesting_ast->rest_expression)
			{
				state->loop_nesting_ast->rest_expression->hir(instructions,
					state);
			}

			if (state->switch_state.is_switch_innermost &&
				mode == ast_break)
			{
				/* Force break out of switch by setting is_break switch state.
				*/
				ir_variable *const is_break_var = state->switch_state.is_break_var;
				ir_dereference_variable *const deref_is_break_var =
					new(ctx)ir_dereference_variable(is_break_var);
				ir_constant *const true_val = new(ctx)ir_constant(true);
				ir_assignment *const set_break_var =
					new(ctx)ir_assignment(deref_is_break_var,
					true_val,
					NULL);

				instructions->push_tail(set_break_var);
			}
			else
			{
				ir_loop_jump *const jump =
					new(ctx)ir_loop_jump((mode == ast_break)
					? ir_loop_jump::jump_break
					: ir_loop_jump::jump_continue);
				instructions->push_tail(jump);
			}
		}

		break;
	}

	/* Jump instructions do not have r-values.
	*/
	return NULL;
}


ir_rvalue * ast_selection_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	ir_rvalue * condition = this->condition->hir(instructions, state);

	/* From page 66 (page 72 of the PDF) of the GLSL 1.50 spec:
	*
	*    "Any expression whose type evaluates to a Boolean can be used as the
	*    conditional expression bool-expression. Vector types are not accepted
	*    as the expression to if."
	*
	* The checks are separated so that higher quality diagnostics can be
	* generated for cases where both rules are violated.
	*/
	if (!condition->type->is_scalar())
	{
		YYLTYPE loc = this->condition->get_location();

		_mesa_glsl_error(&loc, state, "if-statement condition must be scalar ");
	}

	/* GLSL requires conditions be boolean, HLSL doesn't, make it so */
	if (!condition->type->is_boolean())
	{
		YYLTYPE loc = this->condition->get_location();

		apply_type_conversion(glsl_type::bool_type, condition, instructions, state, false, &loc);
	}

	ir_if *const stmt = new(ctx)ir_if(condition);

	if (then_statement != NULL)
	{
		state->symbols->push_scope();
		then_statement->hir(&stmt->then_instructions, state);
		state->symbols->pop_scope();
	}

	if (else_statement != NULL)
	{
		state->symbols->push_scope();
		else_statement->hir(&stmt->else_instructions, state);
		state->symbols->pop_scope();
	}

	instructions->push_tail(stmt);

	/* Process the attribute list
	*/
	foreach_list_const(n, &this->attributes)
	{
		ast_attribute *attrib = exec_node_data(ast_attribute, n, link);

		if (!strcmp(attrib->attribute_name, "branch"))
		{
			if (stmt->mode == ir_if::if_flatten)
			{
				YYLTYPE loc = this->condition->get_location();
				_mesa_glsl_error(&loc, state, "Cannot specify both [branch] and [flatten]");
			}
			stmt->mode = ir_if::if_branch;
		}
		else if (!strcmp(attrib->attribute_name, "flatten"))
		{
			if (stmt->mode == ir_if::if_branch)
			{
				YYLTYPE loc = this->condition->get_location();
				_mesa_glsl_error(&loc, state, "Cannot specify both [branch] and [flatten]");
			}
			stmt->mode = ir_if::if_flatten;
		}
	}

	/* if-statements do not have r-values.
	*/
	return NULL;
}


ir_rvalue * ast_switch_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	/* Track the switch-statement nesting in a stack-like manner.
	*/
	struct glsl_switch_state saved = state->switch_state;

	state->switch_state.is_switch_innermost = true;
	state->switch_state.switch_nesting_ast = this;
	state->switch_state.labels_ht = hash_table_ctor(0, hash_table_pointer_hash,
		hash_table_pointer_compare);
	state->switch_state.previous_default = NULL;

	/* Initalize is_fallthru state to false.
	*/
	ir_rvalue *const is_fallthru_val = new (ctx)ir_constant(false);
	state->switch_state.is_fallthru_var =
		new(ctx)ir_variable(glsl_type::bool_type,
		"switch_is_fallthru_tmp",
		ir_var_temporary);
	instructions->push_tail(state->switch_state.is_fallthru_var);

	ir_dereference_variable *deref_is_fallthru_var =
		new(ctx)ir_dereference_variable(state->switch_state.is_fallthru_var);
	instructions->push_tail(new(ctx)ir_assignment(deref_is_fallthru_var,
		is_fallthru_val,
		NULL));

	/* Initalize is_break state to false.
	*/
	ir_rvalue *const is_break_val = new (ctx)ir_constant(false);
	state->switch_state.is_break_var = new(ctx)ir_variable(glsl_type::bool_type,
		"switch_is_break_tmp",
		ir_var_temporary);
	instructions->push_tail(state->switch_state.is_break_var);

	ir_dereference_variable *deref_is_break_var =
		new(ctx)ir_dereference_variable(state->switch_state.is_break_var);
	instructions->push_tail(new(ctx)ir_assignment(deref_is_break_var,
		is_break_val,
		NULL));

	/* Cache test expression.
	*/
	test_to_hir(instructions, state);

	/* Emit code for body of switch stmt.
	*/
	body->hir(instructions, state);

	hash_table_dtor(state->switch_state.labels_ht);

	state->switch_state = saved;

	/* Switch statements do not have r-values.
	*/
	return NULL;
}


void ast_switch_statement::test_to_hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	/* Cache value of test expression.
	*/
	ir_rvalue* test_expression =
		this->test_expression->hir(instructions, state);

	/* From page 66 (page 55 of the PDF) of the GLSL 1.50 spec:
	*
	*    "The type of init-expression in a switch statement must be a
	*     scalar integer."
	*
	* The checks are separated so that higher quality diagnostics can be
	* generated for cases where the rule is violated.
	*/
	if (!test_expression->type->is_scalar())
	{
		YYLTYPE loc = this->test_expression->get_location();

		_mesa_glsl_error(&loc,
			state,
			"switch-statement expression must be scalar type");
	}

	if (!test_expression->type->is_integer())
	{
		YYLTYPE loc = this->test_expression->get_location();

		_mesa_glsl_warning(&loc,
			state,
			"switch-statement expression should be scalar "
			"integer - casts may not function correctly on non-HLSL platforms");

		switch (test_expression->type->base_type)
		{
		case GLSL_TYPE_FLOAT:
			test_expression = new(ctx)ir_expression(ir_unop_f2i, test_expression);
			break;
		case GLSL_TYPE_HALF:
			test_expression = new(ctx)ir_expression(ir_unop_h2i, test_expression);
			break;
		case GLSL_TYPE_BOOL:
			test_expression = new(ctx)ir_expression(ir_unop_b2i, test_expression);
			break;
		default:
			_mesa_glsl_error(&loc,
				state,
				"switch-statement expression must be numeric type");
			break;
		}
	}

	auto* TestVarType = glsl_type::int_type;
	if (test_expression->type == glsl_type::uint_type)
	{
		TestVarType = glsl_type::uint_type;
	}
	state->switch_state.test_var = new(ctx)ir_variable(TestVarType,
		"switch_test_tmp",
		ir_var_temporary);
	ir_dereference_variable *deref_test_var =
		new(ctx)ir_dereference_variable(state->switch_state.test_var);

	instructions->push_tail(state->switch_state.test_var);
	instructions->push_tail(new(ctx)ir_assignment(deref_test_var,
		test_expression,
		NULL));
}


ir_rvalue * ast_switch_body::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	if (stmts != NULL)
	{
		stmts->hir(instructions, state);
	}

	/* Switch bodies do not have r-values.
	*/
	return NULL;
}


ir_rvalue * ast_case_statement_list::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	foreach_list_typed(ast_case_statement, case_stmt, link, &this->cases)
	{
		case_stmt->hir(instructions, state);
	}

	/* Case statements do not have r-values.
	*/
	return NULL;
}


ir_rvalue * ast_case_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	labels->hir(instructions, state);

	/* Conditionally set fallthru state based on break state.
	*/
	ir_constant *const false_val = new(state)ir_constant(false);
	ir_dereference_variable *const deref_is_fallthru_var =	new(state)ir_dereference_variable(state->switch_state.is_fallthru_var);
	ir_dereference_variable *const deref_is_break_var = new(state)ir_dereference_variable(state->switch_state.is_break_var);
	ir_assignment *const reset_fallthru_on_break = new(state)ir_assignment(deref_is_fallthru_var,
		false_val,
		deref_is_break_var);
	instructions->push_tail(reset_fallthru_on_break);

	/* Guard case statements depending on fallthru state.
	*/
	ir_dereference_variable *const deref_fallthru_guard = new(state)ir_dereference_variable(state->switch_state.is_fallthru_var);
	ir_if *const test_fallthru = new(state)ir_if(deref_fallthru_guard);

	foreach_list_typed(ast_node, stmt, link, &this->stmts)
	{
		stmt->hir(&test_fallthru->then_instructions, state);
	}

	instructions->push_tail(test_fallthru);

	/* Case statements do not have r-values.
	*/
	return NULL;
}


ir_rvalue* ast_case_label_list::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	foreach_list_typed(ast_case_label, label, link, &this->labels)
	{
		label->hir(instructions, state);
	}

	/* Case labels do not have r-values.
	*/
	return NULL;
}


ir_rvalue* ast_case_label::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	ir_dereference_variable *deref_fallthru_var =
		new(ctx)ir_dereference_variable(state->switch_state.is_fallthru_var);

	ir_rvalue *const true_val = new(ctx)ir_constant(true);

	/* If not default case, ...
	*/
	if (this->test_value != NULL)
	{
		/* Conditionally set fallthru state based on
		* comparison of cached test expression value to case label.
		*/
		ir_rvalue *const label_rval = this->test_value->hir(instructions, state);
		ir_constant *label_const = label_rval->constant_expression_value();
		if (!label_const)
		{
			YYLTYPE loc = this->test_value->get_location();

			_mesa_glsl_error(&loc, state,
				"switch statement case label must be a "
				"constant expression");

			/* Stuff a dummy value in to allow processing to continue. */
			label_const = new(ctx)ir_constant(0);
		}
		else
		{
			ast_expression *previous_label = (ast_expression *)
				hash_table_find(state->switch_state.labels_ht,
				(void *)(uintptr_t)label_const->value.u[0]);

			if (previous_label)
			{
				YYLTYPE loc = this->test_value->get_location();
				_mesa_glsl_error(&loc, state,
					"duplicate case value");

				loc = previous_label->get_location();
				_mesa_glsl_error(&loc, state,
					"this is the previous case label");
			}
			else
			{
				hash_table_insert(state->switch_state.labels_ht,
					this->test_value,
					(void *)(uintptr_t)label_const->value.u[0]);
			}
		}

		ir_dereference_variable *deref_test_var =
			new(ctx)ir_dereference_variable(state->switch_state.test_var);

		if (label_const->type != deref_test_var->type)
		{
			// Handle the case of { uint A; switch(A) { case 1: [...] }} as the ir_constant 1 will be int but the A is uint.
			auto* ConvertedValue = convert_component(label_const, deref_test_var->type);
			label_const = ConvertedValue->constant_expression_value();
			// Should NEVER fail!
			check(label_const);
		}

		ir_rvalue *const test_cond = new(ctx)ir_expression(ir_binop_all_equal,
			glsl_type::bool_type,
			label_const,
			deref_test_var);

		ir_assignment *set_fallthru_on_test =
			new(ctx)ir_assignment(deref_fallthru_var,
			true_val,
			test_cond);

		instructions->push_tail(set_fallthru_on_test);
	}
	else
	{ /* default case */
		if (state->switch_state.previous_default)
		{
			printf("a\n");
			YYLTYPE loc = this->get_location();
			_mesa_glsl_error(&loc, state,
				"multiple default labels in one switch");

			printf("b\n");

			loc = state->switch_state.previous_default->get_location();
			_mesa_glsl_error(&loc, state,
				"this is the first default label");
		}
		state->switch_state.previous_default = this;

		/* Set falltrhu state.
		*/
		ir_assignment *set_fallthru =
			new(ctx)ir_assignment(deref_fallthru_var,
			true_val,
			NULL);

		instructions->push_tail(set_fallthru);
	}

	/* Case statements do not have r-values.
	*/
	return NULL;
}


void ast_iteration_statement::condition_to_hir(ir_loop *stmt, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	if (condition != NULL)
	{
		ir_rvalue *const cond =
			condition->hir(&stmt->body_instructions, state);

		if ((cond == NULL)
			|| !cond->type->is_boolean() || !cond->type->is_scalar())
		{
			YYLTYPE loc = condition->get_location();

			_mesa_glsl_error(&loc, state,
				"loop condition must be scalar boolean");
		}
		else
		{
			/* As the first code in the loop body, generate a block that looks
			* like 'if (!condition) break;' as the loop termination condition.
			*/
			ir_rvalue *const not_cond =
				new(ctx)ir_expression(ir_unop_logic_not, glsl_type::bool_type, cond,
				NULL);

			ir_if *const if_stmt = new(ctx)ir_if(not_cond);

			ir_jump *const break_stmt =
				new(ctx)ir_loop_jump(ir_loop_jump::jump_break);

			if_stmt->then_instructions.push_tail(break_stmt);
			stmt->body_instructions.push_tail(if_stmt);
		}
	}
}


ir_rvalue * ast_iteration_statement::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	void *ctx = state;

	/* For-loops and while-loops start a new scope, but do-while loops do not.
	*/
	if (mode != ast_do_while)
	{
		state->symbols->push_scope();
	}

	if (init_statement != NULL)
	{
		check(mode == ast_for);

		exec_list init_ir;
		init_statement->hir(&init_ir, state);

		// Flag all variables declared by the for loop as temporary.
		foreach_iter(exec_list_iterator, iter, init_ir)
		{
			ir_instruction* ir = (ir_instruction*)iter.get();
			ir_variable* var = ir->as_variable();
			if (var)
			{
				var->mode = ir_var_temporary;
			}
		}

		instructions->append_list(&init_ir);
	}

	ir_loop *const stmt = new(ctx)ir_loop();
	instructions->push_tail(stmt);

	/* Track the current loop nesting.
	*/
	ast_iteration_statement *nesting_ast = state->loop_nesting_ast;

	state->loop_nesting_ast = this;

	/* Likewise, indicate that following code is closest to a loop,
	* NOT closest to a switch.
	*/
	bool saved_is_switch_innermost = state->switch_state.is_switch_innermost;
	state->switch_state.is_switch_innermost = false;

	if (mode != ast_do_while)
	{
		condition_to_hir(stmt, state);
	}

	if (body != NULL)
	{
		body->hir(&stmt->body_instructions, state);
	}

	if (rest_expression != NULL)
	{
		rest_expression->hir(&stmt->body_instructions, state);
	}

	if (mode == ast_do_while)
	{
		condition_to_hir(stmt, state);
	}

	if (mode != ast_do_while)
	{
		state->symbols->pop_scope();
	}

	/* Restore previous nesting before returning.
	*/
	state->loop_nesting_ast = nesting_ast;
	state->switch_state.is_switch_innermost = saved_is_switch_innermost;

	/* Process the attribute list
	*/
	foreach_list_const(n, &this->attributes)
	{
		ast_attribute *attrib = exec_node_data(ast_attribute, n, link);

		if (!strcmp(attrib->attribute_name, "loop"))
		{

			//ensure this loop is not receiving conflicting attributes
			if (stmt->mode == ir_loop::loop_unroll)
			{
				YYLTYPE loc = this->get_location();
				_mesa_glsl_error(&loc, state, "Cannot mark a loop with both [unroll] and [loop]");
				return NULL;
			}
			stmt->mode = ir_loop::loop_loop;
		}
		else if (!strcmp(attrib->attribute_name, "unroll"))
		{

			//ensure this loop is not receiving conflicting attributes
			if (stmt->mode == ir_loop::loop_loop)
			{
				YYLTYPE loc = this->get_location();
				_mesa_glsl_error(&loc, state, "Cannot mark a loop with both [unroll] and [loop]");
				return NULL;
			}

			stmt->mode = ir_loop::loop_unroll;

			//look for a count
			if (!attrib->arguments.is_empty())
			{
				int arg_count = 0;
				int arg_val = -1;
				foreach_list_const(m, &attrib->arguments)
				{
					arg_count++;
					ir_rvalue *arg_ir = NULL;
					exec_list dummy_list; /* don't want attributes appending instructions to the real hir */

					ast_attribute_arg *arg = exec_node_data(ast_attribute_arg, m, link);

					if (arg->is_string)
					{
						/* error, string constants not valid for numthreads */
						YYLTYPE loc = this->get_location();
						_mesa_glsl_error(&loc, state, "invalid argument to unroll \"%s\"", arg->argument.string_argument);
						return NULL;
					}
					else
					{
						/* Convert arg to hir to reduce to constant */
						arg_ir = arg->argument.exp_argument->hir(&dummy_list, state);

						if (arg_ir && arg_ir->type->is_integer() && arg_ir->type->is_scalar() &&
							arg_ir->constant_expression_value())
						{
							//should check for negative values?
							arg_val = arg_ir->constant_expression_value()->get_int_component(0);
						}
						else
						{
							/* error, bad format for numthreads argument */
							YYLTYPE loc = this->get_location();
							_mesa_glsl_error(&loc, state, "invalid arg to 'unroll' expected integer constant expression");
							return NULL;
						}
					}
				}
				if (arg_count != 1)
				{
					/* error, too many arguments tounroll */
					YYLTYPE loc = this->get_location();
					_mesa_glsl_error(&loc, state, "incorrect number of arguments to unroll (%d)", arg_count);
					return NULL;
				}
				stmt->unroll_size = arg_val;
			}
			else
			{
				stmt->unroll_size = -1;
			}
		}
		else if (!strcmp(attrib->attribute_name, "fastopt"))
		{
			stmt->fastopt = 1;
		}
		else if (!strcmp(attrib->attribute_name, "allow_uav_condition"))
		{
			/** nothing to do presently */
		}
	}

	/* Loops do not have r-values.
	*/
	return NULL;
}


ir_rvalue * ast_type_specifier::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	if (!this->is_precision_statement && this->structure == NULL)
	{
		return NULL;
	}

	YYLTYPE loc = this->get_location();

	if (this->precision != ast_precision_none
		&& state->language_version != 100
		&& state->language_version < 130)
	{
		_mesa_glsl_error(&loc, state,
			"precision qualifiers exist only in "
			"GLSL ES 1.00, and GLSL 1.30 and later");
		return NULL;
	}
	if (this->precision != ast_precision_none
		&& this->structure != NULL)
	{
		_mesa_glsl_error(&loc, state,
			"precision qualifiers do not apply to structures");
		return NULL;
	}

	/* If this is a precision statement, check that the type to which it is
	* applied is either float or int.
	*
	* From section 4.5.3 of the GLSL 1.30 spec:
	*    "The precision statement
	*       precision precision-qualifier type;
	*    can be used to establish a default precision qualifier. The type
	*    field can be either int or float [...].  Any other types or
	*    qualifiers will result in an error.
	*/
	if (this->is_precision_statement)
	{
		check(this->precision != ast_precision_none);
		check(this->structure == NULL); /* The check for structures was
										* performed above. */
		if (this->is_array)
		{
			_mesa_glsl_error(&loc, state,
				"default precision statements do not apply to "
				"arrays");
			return NULL;
		}
		if (strcmp(this->type_name, "float") != 0 &&
			strcmp(this->type_name, "int") != 0)
		{
			_mesa_glsl_error(&loc, state,
				"default precision statements apply only to types "
				"float and int");
			return NULL;
		}

		/* FINISHME: Translate precision statements into IR. */
		return NULL;
	}

	if (this->structure != NULL)
	{
		return this->structure->hir(instructions, state);
	}

	return NULL;
}


ir_rvalue * ast_struct_specifier::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	unsigned decl_count = 0;

	/* Make an initial pass over the list of structure fields to determine how
	* many there are.  Each element in this list is an ast_declarator_list.
	* This means that we actually need to count the number of elements in the
	* 'declarations' list in each of the elements.
	*/
	foreach_list_typed(ast_declarator_list, decl_list, link, &this->declarations)
	{
		foreach_list_const(decl_ptr, &decl_list->declarations)
		{
			decl_count++;
		}
	}

	/* If this struct inherits from another struct, count the number of members
	* from the parent as they will be inlined.
	*/
	const glsl_type *parent_type = (parent_name != NULL) ? state->symbols->get_type(parent_name) : NULL;

	check((parent_type == NULL) == (parent_name == NULL));
	check(parent_type == NULL || parent_type->is_record());

	decl_count += parent_type ? parent_type->length : 0;

	/* Allocate storage for the structure fields and process the field
	* declarations.  As the declarations are processed, try to also convert
	* the types to HIR.  This ensures that structure definitions embedded in
	* other structure definitions are processed.
	*/
	glsl_struct_field *const fields = ralloc_array(state, glsl_struct_field,
		decl_count);

	unsigned i = 0;

	if (parent_type)
	{
		unsigned j = 0;

		//copy the struct fileds that come from the parent
		for (j = 0; j < parent_type->length; j++)
		{
			fields[i] = parent_type->fields.structure[j];
			i++;
		}
	}

	foreach_list_typed(ast_declarator_list, decl_list, link, &this->declarations)
	{
		const char *type_name;

		decl_list->type->specifier->hir(instructions, state);

		const glsl_type *decl_type =
			decl_list->type->specifier->glsl_type(&type_name, state);

		foreach_list_typed(ast_declaration, decl, link,
			&decl_list->declarations)
		{
			const struct glsl_type *field_type = decl_type;
			if (decl->is_array)
			{
				YYLTYPE loc = decl->get_location();
				field_type = process_array_type(&loc, decl_type, decl->array_size,
					state);
			}
			fields[i].type = (field_type != NULL) ? field_type : glsl_type::error_type;
			fields[i].name = decl->identifier;
			fields[i].semantic = decl->semantic;
			fields[i].centroid = decl_list->type->qualifier.flags.q.centroid;
			fields[i].patchconstant = 0;

			if (decl_list->type->qualifier.flags.q.flat)
			{
				fields[i].interpolation = ir_interp_qualifier_flat;
			}
			else if (decl_list->type->qualifier.flags.q.smooth)
			{
				fields[i].interpolation = ir_interp_qualifier_smooth;
			}
			else if (decl_list->type->qualifier.flags.q.noperspective)
			{
				fields[i].interpolation = ir_interp_qualifier_noperspective;
			}
			else
			{
				fields[i].interpolation = ir_interp_qualifier_none;
			}

			if (decl_list->type->qualifier.flags.q.gs_point)
			{
				fields[i].geometryinput = ir_geometry_input_points;
			}
			else if (decl_list->type->qualifier.flags.q.gs_line)
			{
				fields[i].geometryinput = ir_geometry_input_lines;
			}
			else if (decl_list->type->qualifier.flags.q.gs_lineadj)
			{
				fields[i].geometryinput = ir_geometry_input_lines_adjacent;
			}
			else if (decl_list->type->qualifier.flags.q.gs_triangle)
			{
				fields[i].geometryinput = ir_geometry_input_triangles;
			}
			else if (decl_list->type->qualifier.flags.q.gs_triangleadj)
			{
				fields[i].geometryinput = ir_geometry_input_triangles_adjacent;
			}
			else
			{
				fields[i].geometryinput = ir_geometry_input_none;
			}

			i++;
		}
	}

	check(i == decl_count);

	const glsl_type *t =
		glsl_type::get_record_instance(fields, decl_count, this->name);

	YYLTYPE loc = this->get_location();
	if (!state->symbols->add_type(this->name, t))
	{
		_mesa_glsl_error(&loc, state, "struct '%s' previously defined", this->name);
	}
	else
	{
		const glsl_type **s = reralloc(state, state->user_structures,
			const glsl_type *,
			state->num_user_structures + 1);
		if (s != NULL)
		{
			s[state->num_user_structures] = t;
			state->user_structures = s;
			state->num_user_structures++;
		}
	}

	/* Structure type definitions do not have r-values.
	*/
	return NULL;
}

ir_rvalue * ast_cbuffer_declaration::hir(exec_list *instructions, struct _mesa_glsl_parse_state *state)
{
	for (unsigned block_index = 0; block_index < state->num_uniform_blocks; ++block_index)
	{
		if (strcmp(name, state->uniform_blocks[block_index]->name) == 0)
		{
			YYLTYPE loc = this->get_location();
			_mesa_glsl_error(&loc, state, "uniform block '%s' previously defined", name);
			return NULL;
		}
	}

	unsigned decl_count = 0;
	foreach_list_typed(ast_declarator_list, decl_list, link, &this->declarations)
	{
		foreach_list_const(decl_ptr, &decl_list->declarations)
		{
			decl_count++;
		}
	}

	SCBuffer CBuffer;
	CBuffer.Name = this->name;

	glsl_uniform_block* block = glsl_uniform_block::alloc(state, decl_count);
	block->name = this->name;
	unsigned decl_index = 0;
	foreach_list_typed(ast_declarator_list, decl_list, link, &this->declarations)
	{
		const char *type_name;

		decl_list->type->specifier->hir(instructions, state);
		const glsl_type *decl_type =
			decl_list->type->specifier->glsl_type(&type_name, state);

		foreach_list_typed(ast_declaration, decl, link, &decl_list->declarations)
		{
			const char *field_name = decl->identifier;
			const struct glsl_type *field_type = decl_type;

			if (decl->is_array)
			{
				YYLTYPE loc = decl->get_location();
				field_type = process_array_type(&loc, decl_type, decl->array_size,
					state);
			}
			if (field_type == NULL)
			{
				field_type = glsl_type::error_type;
			}

			ir_variable* var = state->symbols->get_variable(field_name);
			if (var)
			{
				YYLTYPE loc = decl->get_location();
				_mesa_glsl_error(&loc, state,
					"'%s' declared in uniform block '%s' was previously defined",
					field_name, this->name);
			}
			else
			{
				var = new(state)ir_variable(field_type, field_name, ir_var_uniform);
				var->semantic = this->name; // alias semantic to specify the uniform block.
				var->read_only = true;
				state->symbols->add_variable(var);
			}

			check(var);
			check(var->mode == ir_var_uniform);
			check(var->read_only);
			check(strcmp(var->semantic, this->name) == 0);

			if (state->bFlattenUniformBuffers)
			{
				// Promote the member to a global variable
				instructions->push_head(var);
			}

			CBuffer.AddMember(field_type, var);

			block->vars[decl_index] = var;
			decl_index++;
		}
	}

	const glsl_uniform_block **blocks = reralloc(state, state->uniform_blocks,
		const glsl_uniform_block *,
		state->num_uniform_blocks + 1);
	if (blocks != NULL)
	{
		blocks[state->num_uniform_blocks] = block;
		state->uniform_blocks = blocks;
		state->num_uniform_blocks++;
	}

	state->CBuffersOriginal.push_back(CBuffer);

	return NULL;
}

ir_rvalue *ast_initializer_list_expression::hir(
	exec_list *instructions,
	_mesa_glsl_parse_state *state)
{
	YYLTYPE loc = this->get_location();
	_mesa_glsl_error(&loc, state, "initializer lists can only be used to construct variables");
	return ir_rvalue::error_value(state);
}

static unsigned process_initializer_list(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	exec_list *values,
	exec_list *expressions)
{
	unsigned num_values = 0;
	foreach_list_typed(ast_expression, ast, link, expressions)
	{
		if (ast->oper == ast_initializer_list)
		{
			num_values += process_initializer_list(instructions, state, values, &ast->expressions);
		}
		else
		{
			ir_rvalue *result = ast->hir(instructions, state);
			ir_constant *const constant = result->constant_expression_value();
			result = constant ? constant : result;
			values->push_tail(result);
			num_values++;
		}
	}
	return num_values;
}

/**
* This is an O(N^2) operation and could be made O(N) with some work.
*/
static void component_by_component_copy(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	ir_dereference *dest,
	exec_list *values,
	unsigned num_components)
{
	void* ctx = state;
	unsigned dest_offset = 0;

	foreach_iter(exec_list_iterator, iter, *values)
	{
		ir_rvalue* ir = (ir_rvalue*)iter.get();

		// look for an exact match
		ir_rvalue* exact_lhs = access_typed_value_at_offset(dest->clone(ctx, NULL), dest_offset, ir->type);
		if (exact_lhs)
		{
			instructions->push_tail(new(ctx)ir_assignment(exact_lhs, ir->clone(ctx, NULL)));
			dest_offset += exact_lhs->type->component_slots();
		}
		else
		{
			unsigned src_offset = 0;
			unsigned src_components = ir->type->component_slots();
			do
			{
				ir_rvalue* lhs = access_value_at_offset(dest->clone(ctx, NULL), dest_offset++);
				ir_rvalue* rhs = access_value_at_offset(ir->clone(ctx, NULL), src_offset++);
				check(lhs);
				if (rhs->type != lhs->type || (!lhs->type->is_sampler() && !lhs->type->IsSamplerState()))
				{
					rhs = convert_component(rhs, lhs->type);
				}
				instructions->push_tail(new(ctx)ir_assignment(lhs, rhs));
			} while (src_offset < src_components);
		}
	}

	check(dest_offset == num_components);
}

ir_rvalue *ast_initializer_list_expression::initializer_hir(
	exec_list *instructions,
	_mesa_glsl_parse_state *state,
	const glsl_type *initializer_type)
{
	void* ctx = state;
	YYLTYPE loc = this->get_location();

	exec_list init_instructions;
	exec_list init_values;
	process_initializer_list(
		&init_instructions, state, &init_values, &expressions);

	unsigned num_components = 0;
	foreach_iter(exec_list_iterator, iter, init_values)
	{
		ir_rvalue* ir = (ir_rvalue*)iter.get();
		check(((ir_instruction*)ir)->as_rvalue() == ir);
		num_components += ir->type->component_slots();
	}

	if (initializer_type->is_array() && initializer_type->length == 0)
	{
		unsigned elem_components = initializer_type->fields.array->component_slots();
		unsigned array_size = num_components / elem_components;
		if (array_size * elem_components == num_components)
		{
			initializer_type = glsl_type::get_array_instance(
				initializer_type->fields.array, array_size);
		}
	}

	unsigned dest_components = initializer_type->component_slots();
	if (num_components != dest_components)
	{
		_mesa_glsl_error(&loc, state, "initializer doesn't match type");
		return ir_rvalue::error_value(ctx);
	}

	num_components = 0;
	bool all_constant = true;
	foreach_iter(exec_list_iterator, iter, init_values)
	{
		ir_rvalue* ir = (ir_rvalue*)iter.get();
		check(((ir_instruction*)ir)->as_rvalue() == ir);
		num_components += ir->type->component_slots();

		all_constant &= ir->as_constant() != NULL;

		if (ir->as_dereference() == NULL && ir->as_constant() == NULL)
		{
			ir_variable* var = new(ctx)ir_variable(ir->type, NULL, ir_var_temporary);
			init_instructions.push_tail(var);
			init_instructions.push_tail(new(ctx)ir_assignment(
				new(ctx)ir_dereference_variable(var),
				ir));
			ir->replace_with(new(ctx)ir_dereference_variable(var));
		}
	}
	check(num_components == dest_components);

	// EHartNV
	// Detect if the initializer was all constants
	// If it was, create an ir_constant from the exec list
	// Records and multidimensional arrays are not currently supported.

	ir_rvalue* ret = NULL;

	if (!all_constant || initializer_type->is_record() || (initializer_type->is_array() && initializer_type->fields.array->is_array()))
	{
		ir_variable* tmp_var = new(ctx)ir_variable(initializer_type, NULL, ir_var_temporary);
		ir_dereference_variable* tmp_dref = new(ctx)ir_dereference_variable(tmp_var);

		init_instructions.push_tail(tmp_var);
		component_by_component_copy(&init_instructions, state, tmp_dref, &init_values, num_components);
		ret = tmp_dref;
	}
	else
	{
		ret = new(ctx)ir_constant(initializer_type, &init_values);
	}

	instructions->append_list(&init_instructions);

	return ret;
}

class ir_remove_matrix_swizzle_visitor : public ir_rvalue_visitor
{
public:
	ir_visitor_status visit_leave(ir_assignment *assign)
	{
		ir_visitor_status status = ir_rvalue_visitor::visit_leave(assign);

		ir_dereference* lhs = assign->lhs;
		if (lhs->type->is_matrix() && assign->write_mask != 0)
		{
			check(lhs->type->base_type == assign->rhs->type->base_type);
			check(assign->rhs->type->is_scalar() || assign->rhs->type->is_vector());

			void* ctx = ralloc_parent(assign);
			ir_variable* dest_var = lhs->variable_referenced();
			ir_variable* tmp_vec = new(ctx)ir_variable(assign->rhs->type, NULL, ir_var_temporary);

			this->base_ir->insert_before(tmp_vec);
			this->base_ir->insert_before(new(ctx)ir_assignment(
				new(ctx)ir_dereference_variable(tmp_vec),
				assign->rhs));

			unsigned write_mask = assign->write_mask;
			unsigned src_index = 0;
			unsigned dest_index = 0;
			do
			{
				if (write_mask & 0x1)
				{
					ir_dereference_array* Row = new(ctx) ir_dereference_array(dest_var, new(ctx) ir_constant(dest_index / dest_var->type->vector_elements));
					ir_dereference_array* LHS =  new(ctx) ir_dereference_array(
									Row,
									new(ctx) ir_constant(dest_index % dest_var->type->vector_elements));
					ir_dereference* RHS = assign->rhs->type->is_vector()
						? (ir_dereference*)(new(ctx) ir_dereference_array(tmp_vec, new(ctx)ir_constant(src_index)))
						: (ir_dereference*)(new(ctx) ir_dereference_variable(tmp_vec));
					this->base_ir->insert_before(new(ctx)ir_assignment(LHS, RHS));
					src_index++;
				}
				dest_index++;
				write_mask >>= 1;
			} while (write_mask);

			assign->lhs = new(ctx)ir_dereference_variable(dest_var);
			assign->rhs = new(ctx)ir_dereference_variable(dest_var);
			assign->write_mask = 0;
		}

		return status;
	}

	virtual void handle_rvalue(ir_rvalue **rvalue)
	{
		ir_swizzle* swizzle = (rvalue && *rvalue) ? (*rvalue)->as_swizzle() : NULL;
		if (swizzle && swizzle->val->type->is_matrix())
		{
			void* ctx = ralloc_parent(swizzle);
			ir_variable* src_var = NULL;
			ir_dereference_variable* src_deref = swizzle->val->as_dereference_variable();
			if (src_deref)
			{
				src_var = src_deref->var;
			}
			else
			{
				src_var = new(ctx)ir_variable(swizzle->val->type, NULL, ir_var_temporary);
				this->base_ir->insert_before(src_var);
				this->base_ir->insert_before(new(ctx)ir_assignment(
					new(ctx)ir_dereference_variable(src_var),
					swizzle->val));
			}

			unsigned components[4] =
			{
				swizzle->mask.x, swizzle->mask.y, swizzle->mask.z, swizzle->mask.w
			};
			ir_variable* var = new(ctx)ir_variable(swizzle->type, NULL, ir_var_temporary);
			this->base_ir->insert_before(var);

			//			check( (swizzle->type->vector_elements * swizzle->type->matrix_columns) <= swizzle->mask.num_components );
			// Commented out, as we can't do asserts here, and can't do _mesa_glsl_error.

			for (unsigned i = 0; i < swizzle->mask.num_components; ++i)
			{
				this->base_ir->insert_before(new(ctx)ir_assignment(
					(swizzle->type->vector_elements == 1 && swizzle->type->matrix_columns == 1)
					? (ir_dereference*)new(ctx)ir_dereference_variable(var)
					: (ir_dereference*)new(ctx)ir_dereference_array(var, new(ctx)ir_constant(i)),
					new(ctx)ir_dereference_array(
					new(ctx)ir_dereference_array(
					src_var,
					new(ctx)ir_constant(components[i] / src_var->type->vector_elements)
					),
					new(ctx)ir_constant(components[i] % src_var->type->vector_elements)
					)
					));
			}
			*rvalue = new(ctx)ir_dereference_variable(var);
		}
	}
};

static void remove_matrix_swizzles(exec_list *instructions)
{
	ir_remove_matrix_swizzle_visitor v;
	visit_list_elements(&v, instructions);
}
