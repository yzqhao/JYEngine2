
#pragma once
#ifndef LOOP_ANALYSIS_H
#define LOOP_ANALYSIS_H

#include "ir.h"
#include "hash_table.h"

/**
 * Analyze and classify all variables used in all loops in the instruction list
 */
extern class loop_state *
analyze_loop_variables(exec_list *instructions);


/**
 * Fill in loop control fields
 *
 * Based on analysis of loop variables, this function tries to remove sequences
 * in the loop of the form
 *
 *  (if (expression bool ...) (break))
 *
 * and fill in the \c ir_loop::from, \c ir_loop::to, and \c ir_loop::counter
 * fields of the \c ir_loop.
 *
 * In this process, some conditional break-statements may be eliminated
 * altogether.  For example, if it is provable that one loop exit condition will
 * always be satisfied before another, the unnecessary exit condition will be
 * removed.
 */
extern bool
set_loop_controls(exec_list *instructions, loop_state *ls);


extern bool
unroll_loops(exec_list *instructions, loop_state *ls, unsigned max_iterations, _mesa_glsl_parse_state *state);


/**
 * Tracking for all variables used in a loop
 */
class loop_variable_state : public exec_node {
public:
   class loop_variable *get(const ir_variable *);
   class loop_variable *insert(ir_variable *);
   class loop_terminator *insert(ir_if *);


   /**
    * Loop whose variable state is being tracked by this structure
    */
   ir_loop *loop;

   /**
    * Variables that have not yet been classified
    */
   exec_list variables;

   /**
    * Variables whose values are constant within the body of the loop
    *
    * This list contains \c loop_variable objects.
    */
   exec_list constants;

   /**
    * Induction variables for this loop
    *
    * This list contains \c loop_variable objects.
    */
   exec_list induction_variables;

   /**
    * Simple if-statements that lead to the termination of the loop
    *
    * This list contains \c loop_terminator objects.
    *
    * \sa is_loop_terminator
    */
   exec_list terminators;

   /**
    * Hash table containing all variables accessed in this loop
    */
   hash_table *var_hash;

   /**
    * Maximum number of loop iterations.
    *
    * If this value is negative, then the loop may be infinite.  This actually
    * means that analysis was unable to determine an upper bound on the number
    * of loop iterations.
    */
   int max_iterations;

   /**
    * Number of ir_loop_jump instructions that operate on this loop
    */
   unsigned num_loop_jumps;

   /**
    * Whether this loop contains any function calls.
    */
   bool contains_calls;

   /**
    * Whether the calls are all built-in
    */
   bool only_builtin_calls;

   loop_variable_state()
   {
      this->max_iterations = -1;
      this->num_loop_jumps = 0;
      this->contains_calls = false;
	  this->only_builtin_calls = true;
      this->var_hash = hash_table_ctor(0, ir_hash_table_pointer_hash,
				       ir_hash_table_pointer_compare);
   }

   ~loop_variable_state()
   {
      hash_table_dtor(this->var_hash);
   }
};


class loop_variable : public exec_node {
public:
   /** The variable in question. */
   ir_variable *var;

   /** Is the variable read in the loop before it is written? */
   bool read_before_write;

   /** Are all variables in the RHS of the assignment loop constants? */
   bool rhs_clean;

   /** Is this the result of an atomic? */
   bool atomic_result;

   /** Is there an assignment to the variable that is conditional? */
   bool conditional_assignment;

   /** Reference to the first assignment to the variable in the loop body. */
   ir_assignment *first_assignment;

   /** Number of assignments to the variable in the loop body. */
   unsigned num_assignments;

   /**
    * Increment values for loop induction variables
    *
    * Loop induction variables have a single increment of the form
    * \c b * \c biv + \c c, where \c b and \c c are loop constants and \c i
    * is a basic loop induction variable.
    *
    * If \c iv_scale is \c NULL, 1 is used.  If \c biv is the same as \c var,
    * then \c var is a basic loop induction variable.
    */
   /*@{*/
   ir_rvalue *iv_scale;
   ir_variable *biv;
   ir_rvalue *increment;
   /*@}*/


   inline bool is_loop_constant() const
   {
      const bool is_const = (this->num_assignments == 0)
	 || ((this->num_assignments == 1)
	     && !this->conditional_assignment
	     && !this->read_before_write
		 && !this->atomic_result
	     && this->rhs_clean);

      /* If the RHS of *the* assignment is clean, then there must be exactly
       * one assignment of the variable.
       */
      check((this->rhs_clean && (this->num_assignments == 1))
	     || !this->rhs_clean);

      /* Variables that are marked read-only *MUST* be loop constant.
       */
	  // @todo: Need to vastly improve loop analysis code algorithm in order for this check to be valid in our typical
	  // use cases; for example it will fail on:
	  // X(F)
	  // {
	  //	for (i = 0;i < n; ++i)
	  //	{
	  //		const V = F * i;	<-- F is non-const as it's a function param; i is a loop variable; analysis doesn't handle this case
	  //	}
	  // }
      //check(!this->var->read_only || (this->var->read_only && is_const));

      return is_const;
   }
};


class loop_terminator : public exec_node {
public:
   ir_if *ir;
};


class loop_state {
public:
   ~loop_state();

   /**
    * Get the loop variable state data for a particular loop
    */
   loop_variable_state *get(const ir_loop *);

   loop_variable_state *insert(ir_loop *ir);

   bool loop_found;

private:
   loop_state();

   /**
    * Hash table containing all loops that have been analyzed.
    */
   hash_table *ht;

   void *mem_ctx;

   friend class loop_analysis;
};

#endif /* LOOP_ANALYSIS_H */
