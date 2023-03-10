

#include "ShaderCompilerCommon.h"
#include "glsl_types.h"
#include "loop_analysis.h"
#include "ir_hierarchical_visitor.h"
#include "glsl_parser_extras.h"

/* Limit the number of iterations when force unrolling a loop to prevent crashes due
* to failing to find a loop terminator.
*/
#define MAX_ITERATIONS_FAILSAFE 1024

class loop_unroll_visitor : public ir_hierarchical_visitor
{
public:
	loop_unroll_visitor(loop_state *state, unsigned max_iterations, _mesa_glsl_parse_state *parser_state)
	{
		this->state = state;
		this->progress = false;
		this->max_iterations = max_iterations;
		this->parser_state = parser_state;
	}

	virtual ir_visitor_status visit_leave(ir_loop *ir);

	loop_state *state;
	_mesa_glsl_parse_state *parser_state;

	bool progress;
	unsigned max_iterations;
};


static bool
is_break(ir_instruction *ir)
{
	return ir != NULL && ir->ir_type == ir_type_loop_jump
		&& ((ir_loop_jump *)ir)->is_break();
}

class loop_unroll_count : public ir_hierarchical_visitor
{
public:
	int nodes;
	bool fail;

	loop_unroll_count(exec_list *list)
	{
		nodes = 0;
		fail = false;

		run(list);
	}

	virtual ir_visitor_status visit_enter(ir_assignment *ir)
	{
		nodes++;
		return visit_continue;
	}

	virtual ir_visitor_status visit_enter(ir_expression *ir)
	{
		nodes++;
		return visit_continue;
	}

	virtual ir_visitor_status visit_enter(ir_loop *ir)
	{
		fail = true;
		return visit_continue;
	}
};


ir_visitor_status
loop_unroll_visitor::visit_leave(ir_loop *ir)
{
	loop_variable_state *const ls = this->state->get(ir);
	int iterations;
	int max_allowed_iterations = this->max_iterations;
	bool force_unroll = (ir->mode == ir_loop::loop_unroll && ir->unroll_size == -1);

	/* If the programmer requested we generate a loop, don't try to unroll. */
	if (ir->mode == ir_loop::loop_loop)
	{
		return visit_continue;
	}

	/* If the programmer specified a max unroll size, respect it. */
	if (ir->mode == ir_loop::loop_unroll && ir->unroll_size > 0)
	{
		max_allowed_iterations = ir->unroll_size;
	}

	/* If we've entered a loop that hasn't been analyzed, something really,
	* really bad has happened.
	*/
	if (ls == NULL)
	{
		check(ls != NULL);
		return visit_continue;
	}

	iterations = ls->max_iterations;

	/* Don't try to unroll loops where the number of iterations is not known
	* at compile-time.
	*/
	if (iterations < 0)
	{
		return visit_continue;
	}

	/* Don't try to unroll loops that have zillions of iterations either.
	*/
	if (!force_unroll && iterations > max_allowed_iterations)
		return visit_continue;

	/* Prevent infinite loop unrolls, if hlslcc isn't finding a terminating condition
	*/
	if (force_unroll && iterations > MAX_ITERATIONS_FAILSAFE)
	{
		/* EHartNV - this should probably be changed to a warning in the future. For
		* now, the error helps identify cases where HLSLCC may be failing to properly
		* interpret the loop
		*/
		_mesa_glsl_warning(parser_state, "Forced unroll exceeded %d iterations, possible infinite loop ", MAX_ITERATIONS_FAILSAFE);
		return visit_continue;
	}

	/* Don't try to unroll nested loops and loops with a huge body.
	*/
	loop_unroll_count count(&ir->body_instructions);

	if (count.fail)
	{
		return visit_continue;
	}

	if (!force_unroll && count.nodes * iterations > max_allowed_iterations * 5)
	{
		return visit_continue;
	}

	if (ls->num_loop_jumps > 1)
	{
		return visit_continue;
	}
	else if (ls->num_loop_jumps)
	{
		ir_instruction *last_ir = (ir_instruction *)ir->body_instructions.get_tail();
		check(last_ir != NULL);

		if (is_break(last_ir))
		{
			/* If the only loop-jump is a break at the end of the loop, the loop
			* will execute exactly once.  Remove the break, set the iteration
			* count, and fall through to the normal unroller.
			*/
			last_ir->remove();
			iterations = 1;

			this->progress = true;
		}
		else
		{
			ir_if *ir_if = NULL;
			ir_instruction *break_ir = NULL;
			bool continue_from_then_branch = false;

			foreach_list(node, &ir->body_instructions)
			{
				/* recognize loops in the form produced by ir_lower_jumps */
				ir_instruction *cur_ir = (ir_instruction *)node;

				ir_if = cur_ir->as_if();
				if (ir_if != NULL)
				{
					/* Determine which if-statement branch, if any, ends with a
					* break.  The branch that did *not* have the break will get a
					* temporary continue inserted in each iteration of the loop
					* unroll.
					*
					* Note that since ls->num_loop_jumps is <= 1, it is impossible
					* for both branches to end with a break.
					*/
					ir_instruction *ir_if_last =
						(ir_instruction *)ir_if->then_instructions.get_tail();

					if (is_break(ir_if_last))
					{
						continue_from_then_branch = false;
						break_ir = ir_if_last;
						break;
					}
					else
					{
						ir_if_last =
							(ir_instruction *)ir_if->else_instructions.get_tail();

						if (is_break(ir_if_last))
						{
							break_ir = ir_if_last;
							continue_from_then_branch = true;
							break;
						}
					}
				}
			}

			if (break_ir == NULL)
			{
				return visit_continue;
			}

			/* move instructions after then if in the continue branch */
			while (!ir_if->get_next()->is_tail_sentinel())
			{
				ir_instruction *move_ir = (ir_instruction *)ir_if->get_next();

				move_ir->remove();
				if (continue_from_then_branch)
				{
					ir_if->then_instructions.push_tail(move_ir);
				}
				else
				{
					ir_if->else_instructions.push_tail(move_ir);
				}
			}

			/* Remove the break from the if-statement.
			*/
			break_ir->remove();

			void *const mem_ctx = ralloc_parent(ir);
			ir_instruction *ir_to_replace = ir;

			for (int i = 0; i < iterations; i++)
			{
				exec_list copy_list;

				copy_list.make_empty();
				clone_ir_list(mem_ctx, &copy_list, &ir->body_instructions);

				ir_if = ((ir_instruction *)copy_list.get_tail())->as_if();
				check(ir_if != NULL);

				ir_to_replace->insert_before(&copy_list);
				ir_to_replace->remove();

				/* placeholder that will be removed in the next iteration */
				ir_to_replace =
					new(mem_ctx)ir_loop_jump(ir_loop_jump::jump_continue);

				exec_list *const list = (continue_from_then_branch)
					? &ir_if->then_instructions : &ir_if->else_instructions;

				list->push_tail(ir_to_replace);
			}

			ir_to_replace->remove();

			this->progress = true;
			return visit_continue;
		}
	}

	void *const mem_ctx = ralloc_parent(ir);

	for (int i = 0; i < iterations; i++)
	{
		exec_list copy_list;

		copy_list.make_empty();
		clone_ir_list(mem_ctx, &copy_list, &ir->body_instructions);

		ir->insert_before(&copy_list);
	}

	/* The loop has been replaced by the unrolled copies.  Remove the original
	* loop from the IR sequence.
	*/
	ir->remove();

	this->progress = true;
	return visit_continue;
}


bool
unroll_loops(exec_list *instructions, loop_state *ls, unsigned max_iterations, _mesa_glsl_parse_state * state)
{
	loop_unroll_visitor v(ls, max_iterations, state);

	v.run(instructions);

	return v.progress;
}
