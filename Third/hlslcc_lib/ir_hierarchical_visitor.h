
#pragma once
#ifndef IR_HIERARCHICAL_VISITOR_H
#define IR_HIERARCHICAL_VISITOR_H

/**
 * Enumeration values returned by visit methods to guide processing
 */
enum ir_visitor_status {
   visit_continue,		/**< Continue visiting as normal. */
   visit_continue_with_parent,	/**< Don't visit siblings, continue w/parent. */
   visit_stop			/**< Stop visiting immediately. */
};


/**
 * Base class of hierarchical visitors of IR instruction trees
 *
 * Hierarchical visitors differ from traditional visitors in a couple of
 * important ways.  Rather than having a single \c visit method for each
 * subclass in the composite, there are three kinds of visit methods.
 * Leaf-node classes have a traditional \c visit method.  Internal-node
 * classes have a \c visit_enter method, which is invoked just before
 * processing child nodes, and a \c visit_leave method which is invoked just
 * after processing child nodes.
 *
 * In addition, each visit method and the \c accept methods in the composite
 * have a return value which guides the navigation.  Any of the visit methods
 * can choose to continue visiting the tree as normal (by returning \c
 * visit_continue), terminate visiting any further nodes immediately (by
 * returning \c visit_stop), or stop visiting sibling nodes (by returning \c
 * visit_continue_with_parent).
 *
 * These two changes combine to allow nagivation of children to be implemented
 * in the composite's \c accept method.  The \c accept method for a leaf-node
 * class will simply call the \c visit method, as usual, and pass its return
 * value on.  The \c accept method for internal-node classes will call the \c
 * visit_enter method, call the \c accpet method of each child node, and,
 * finally, call the \c visit_leave method.  If any of these return a value
 * other that \c visit_continue, the correct action must be taken.
 *
 * The final benefit is that the hierarchical visitor base class need not be
 * abstract.  Default implementations of every \c visit, \c visit_enter, and
 * \c visit_leave method can be provided.  By default each of these methods
 * simply returns \c visit_continue.  This allows a significant reduction in
 * derived class code.
 *
 * For more information about hierarchical visitors, see:
 *
 *    http://c2.com/cgi/wiki?HierarchicalVisitorPattern
 *    http://c2.com/cgi/wiki?HierarchicalVisitorDiscussion
 */

class ir_hierarchical_visitor {
public:
   ir_hierarchical_visitor();
   virtual ~ir_hierarchical_visitor() {}

   /**
	* \name Visit methods for leaf-node classes
	*/
   /*@{*/
   virtual ir_visitor_status visit(class ir_rvalue *);
   virtual ir_visitor_status visit(class ir_variable *);
   virtual ir_visitor_status visit(class ir_constant *);
   virtual ir_visitor_status visit(class ir_loop_jump *);

   /**
	* ir_dereference_variable isn't technically a leaf, but it is treated as a
	* leaf here for a couple reasons.  By not automatically visiting the one
	* child ir_variable node from the ir_dereference_variable, ir_variable
	* nodes can always be handled as variable declarations.  Code that used
	* non-hierarchical visitors had to set an "in a dereference" flag to
	* determine how to handle an ir_variable.  By forcing the visitor to
	* handle the ir_variable within the ir_dereference_variable visitor, this
	* kludge can be avoided.
	*
	* In addition, I can envision no use for having separate enter and leave
	* methods.  Anything that could be done in the enter and leave methods
	* that couldn't just be done in the visit method.
	*/
   virtual ir_visitor_status visit(class ir_dereference_variable *);
   /*@}*/

   /**
	* \name Visit methods for internal-node classes
	*/
   /*@{*/
   virtual ir_visitor_status visit_enter(class ir_loop *);
   virtual ir_visitor_status visit_leave(class ir_loop *);
   virtual ir_visitor_status visit_enter(class ir_function_signature *);
   virtual ir_visitor_status visit_leave(class ir_function_signature *);
   virtual ir_visitor_status visit_enter(class ir_function *);
   virtual ir_visitor_status visit_leave(class ir_function *);
   virtual ir_visitor_status visit_enter(class ir_expression *);
   virtual ir_visitor_status visit_leave(class ir_expression *);
   virtual ir_visitor_status visit_enter(class ir_texture *);
   virtual ir_visitor_status visit_leave(class ir_texture *);
   virtual ir_visitor_status visit_enter(class ir_swizzle *);
   virtual ir_visitor_status visit_leave(class ir_swizzle *);
   virtual ir_visitor_status visit_enter(class ir_dereference_array *);
   virtual ir_visitor_status visit_leave(class ir_dereference_array *);
   virtual ir_visitor_status visit_enter(class ir_dereference_image *);
   virtual ir_visitor_status visit_leave(class ir_dereference_image *);
   virtual ir_visitor_status visit_enter(class ir_dereference_record *);
   virtual ir_visitor_status visit_leave(class ir_dereference_record *);
   virtual ir_visitor_status visit_enter(class ir_assignment *);
   virtual ir_visitor_status visit_leave(class ir_assignment *);
   virtual ir_visitor_status visit_enter(class ir_call *);
   virtual ir_visitor_status visit_leave(class ir_call *);
   virtual ir_visitor_status visit_enter(class ir_return *);
   virtual ir_visitor_status visit_leave(class ir_return *);
   virtual ir_visitor_status visit_enter(class ir_discard *);
   virtual ir_visitor_status visit_leave(class ir_discard *);
   virtual ir_visitor_status visit_enter(class ir_if *);
   virtual ir_visitor_status visit_leave(class ir_if *);
   virtual ir_visitor_status visit_enter(class ir_atomic *);
   virtual ir_visitor_status visit_leave(class ir_atomic *);
   /*@}*/


   /**
	* Utility function to process a linked list of instructions with a visitor
	*/
   void run(struct exec_list *instructions);

   /* Some visitors may need to insert new variable declarations and
	* assignments for portions of a subtree, which means they need a
	* pointer to the current instruction in the stream, not just their
	* node in the tree rooted at that instruction.
	*
	* This is implemented by visit_list_elements -- if the visitor is
	* not called by it, nothing good will happen.
	*/
   class ir_instruction *base_ir;

   /**
	* Callback function that is invoked on entry to each node visited.
	*
	* \warning
	* Visitor classes derived from \c ir_hierarchical_visitor \b may \b not
	* invoke this function.  This can be used, for example, to cause the
	* callback to be invoked on every node type execpt one.
	*/
   void (*callback)(class ir_instruction *ir, void *data);

   /**
	* Extra data parameter passed to the per-node callback function
	*/
   void *data;

   /**
	* Currently in the LHS of an assignment?
	*
	* This is set and cleared by the \c ir_assignment::accept method.
	*/
   bool in_assignee;
};

void visit_tree(ir_instruction *ir,
		void (*callback)(class ir_instruction *ir, void *data),
		void *data);

ir_visitor_status visit_list_elements(ir_hierarchical_visitor *v, exec_list *l,
									  bool statement_list = true);

// Using bUnsafe will first make a list of intructions to traverse, then execute them; this is helpful on situations where
// we want to replace an existing instruction but can't modify it during the visitor.
ir_visitor_status VisitRange(class ir_visitor* Visitor, ir_instruction* IRFirst, ir_instruction* IRLast, bool bUnsafe = false);
ir_visitor_status VisitRange(ir_hierarchical_visitor* Visitor, ir_instruction* IRFirst, ir_instruction* IRLast, bool bUnsafe = false);

#endif /* IR_HIERARCHICAL_VISITOR_H */
