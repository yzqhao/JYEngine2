
#pragma once
#ifndef LIST_CONTAINER_H
#define LIST_CONTAINER_H

#include "ralloc.h"

struct exec_node {
   struct exec_node *next;
   struct exec_node *prev;

#ifdef __cplusplus
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *node;

      node = ralloc_size(ctx, size);
      //check(node != nullptr);

      return node;
   }

   /* If the user *does* call delete, that's OK, we will just
    * ralloc_free in that case. */
   static void operator delete(void *node)
   {
      ralloc_free(node);
   }

   exec_node() : next(nullptr), prev(nullptr)
   {
      /* empty */
   }

   const exec_node *get_next() const
   {
      return next;
   }

   exec_node *get_next()
   {
      return next;
   }

   const exec_node *get_prev() const
   {
      return prev;
   }

   exec_node *get_prev()
   {
      return prev;
   }

   void remove()
   {
      check(next);
	  next->prev = prev;
      check(prev);
	  prev->next = next;
	  
      next = nullptr;
      prev = nullptr;
   }

   /**
    * Link a node with itself
    *
    * This creates a sort of degenerate list that is occasionally useful.
    */
   void self_link()
   {
      next = this;
      prev = this;
   }

   /**
    * Insert a node in the list after the current node
    */
   void insert_after(exec_node *after)
   {
      after->next = this->next;
      after->prev = this;

      check(this->next);
	  this->next->prev = after;
	  this->next = after;
   }
   /**
    * Insert a node in the list before the current node
    */
   void insert_before(exec_node *before)
   {
      before->next = this;
      before->prev = this->prev;

      check(this->prev);
	  this->prev->next = before;
	  this->prev = before;
   }

   /**
    * Insert another list in the list before the current node
    */
   void insert_before(struct exec_list *before);

   /**
    * Replace the current node with the given node.
    */
   void replace_with(exec_node *replacement)
   {
      replacement->prev = this->prev;
      replacement->next = this->next;

      check(this->prev);
	  this->prev->next = replacement;
      check(this->next);
	  this->next->prev = replacement;
   }

   /**
    * Is this the sentinel at the tail of the list?
    */
   bool is_tail_sentinel() const
   {
      return this->next == nullptr;
   }

   /**
    * Is this the sentinel at the head of the list?
    */
   bool is_head_sentinel() const
   {
      return this->prev == nullptr;
   }

   bool check_invariants() const
   {
	   if (prev && prev->next != this)
	   {
		   return false;
	   }
	   if (next && next->prev != this)
	   {
		   return false;
	   }
	   return true;
   }
#endif
};


#ifdef __cplusplus
/* This macro will not work correctly if 't' uses virtual inheritance.  If you
 * are using virtual inheritance, you deserve a slow and painful death.  Enjoy!
 */
#define exec_list_offsetof(t, f, p) \
   (((char *) &((t *) p)->f) - ((char *) p))
#else
#define exec_list_offsetof(t, f, p) offsetof(t, f)
#endif

/**
 * Get a pointer to the structure containing an exec_node
 *
 * Given a pointer to an \c exec_node embedded in a structure, get a pointer to
 * the containing structure.
 *
 * \param type  Base type of the structure containing the node
 * \param node  Pointer to the \c exec_node
 * \param field Name of the field in \c type that is the embedded \c exec_node
 */
#define exec_node_data(type, node, field) \
   ((type *) (((char *) node) - exec_list_offsetof(type, field, node)))

#ifdef __cplusplus
struct exec_node;

class iterator {
public:
   void next()
   {
   }

   void *get()
   {
      return nullptr;
   }

   bool has_next() const
   {
      return false;
   }
};

class exec_list_iterator : public iterator {
public:
   exec_list_iterator(exec_node *n) : node(n), _next(n->next)
   {
      /* empty */
   }

   void next()
   {
      node = _next;
      _next = node->next;
   }

   void remove()
   {
      node->remove();
   }

   exec_node *get()
   {
      return node;
   }

   bool has_next() const
   {
      return _next != nullptr;
   }

private:
   exec_node *node;
   exec_node *_next;
};

#define foreach_iter(iter_type, iter, container) \
   for (iter_type iter = (container) . iterator(); iter.has_next(); iter.next())
#endif


struct exec_list {
   struct exec_node *head;
   struct exec_node *tail;
   struct exec_node *tail_pred;

#ifdef __cplusplus
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *node;

      node = ralloc_size(ctx, size);
      //check(node != nullptr);

      return node;
   }

   /* If the user *does* call delete, that's OK, we will just
    * ralloc_free in that case. */
   static void operator delete(void *node)
   {
      ralloc_free(node);
   }

   exec_list()
   {
      make_empty();
   }

   void make_empty()
   {
      head = (exec_node *) & tail;
      tail = nullptr;
      tail_pred = (exec_node *) & head;
   }

   bool is_empty() const
   {
      /* There are three ways to test whether a list is empty or not.
       *
       * - Check to see if the \c head points to the \c tail.
       * - Check to see if the \c tail_pred points to the \c head.
       * - Check to see if the \c head is the sentinel node by test whether its
       *   \c next pointer is \c nullptr.
       *
       * The first two methods tend to generate better code on modern systems
       * because they save a pointer dereference.
       */
      // Cast to volatile is needed to prevent clang 3.8+ from optimizing is_empty() away in MoveGlobalInstructionsToMain().
      // Consider a list that was created (make_empty()) and only push_tail()-ed to. Without this clang considers is_empty() to be true
      // because it never sees head being updated (it is updated indirectly through n->prev->next = n; assignment in push_tail()).
      return *((volatile exec_node **)&head) == (exec_node *) &tail;
   }

   const exec_node *get_head() const
   {
      return !is_empty() ? head : nullptr;
   }

   exec_node *get_head()
   {
      return !is_empty() ? head : nullptr;
   }

   const exec_node *get_tail() const
   {
      return !is_empty() ? tail_pred : nullptr;
   }

   exec_node *get_tail()
   {
      return !is_empty() ? tail_pred : nullptr;
   }

   void push_head(exec_node *n)
   {
      n->next = head;
      n->prev = (exec_node *) &head;

      n->next->prev = n;
      head = n;
   }

   void push_tail(exec_node *n)
   {
      n->next = (exec_node *) &tail;
      n->prev = tail_pred;

      n->prev->next = n;
      tail_pred = n;
   }

   void push_degenerate_list_at_head(exec_node *n)
   {
      //check(n->prev->next == n);

      n->prev->next = head;
      head->prev = n->prev;
      n->prev = (exec_node *) &head;
      head = n;
   }

   /**
    * Remove the first node from a list and return it
    *
    * \return
    * The first node in the list or \c nullptr if the list is empty.
    *
    * \sa exec_list::get_head
    */
   exec_node *pop_head()
   {
      exec_node *const n = this->get_head();
      if (n != nullptr)
	 n->remove();

      return n;
   }

   /**
    * Move all of the nodes from this list to the target list
    */
   void move_nodes_to(exec_list *target)
   {
      if (is_empty()) {
	 target->make_empty();
      } else {
	 target->head = head;
	 target->tail = nullptr;
	 target->tail_pred = tail_pred;

	 target->head->prev = (exec_node *) &target->head;
	 target->tail_pred->next = (exec_node *) &target->tail;

	 make_empty();
      }
   }

   /**
    * Append all nodes from the source list to the target list
    */
   void
   append_list(exec_list *source)
   {
      if (source->is_empty())
	 return;

      /* Link the first node of the source with the last node of the target list.
       */
      this->tail_pred->next = source->head;
      source->head->prev = this->tail_pred;

      /* Make the tail of the source list be the tail of the target list.
       */
      this->tail_pred = source->tail_pred;
      this->tail_pred->next = (exec_node *) &this->tail;

      /* Make the source list empty for good measure.
       */
      source->make_empty();
   }

   exec_list_iterator iterator()
   {
      return exec_list_iterator(head);
   }

   exec_list_iterator iterator() const
   {
      return exec_list_iterator((exec_node *) head);
   }
#endif
};


#ifdef __cplusplus
inline void exec_node::insert_before(exec_list *before)
{
   if (before->is_empty())
      return;

   before->tail_pred->next = this;
   before->head->prev = this->prev;

   this->prev->next = before->head;
   this->prev = before->tail_pred;

   before->make_empty();
}
#endif

/**
 * This version is safe even if the current node is removed.
 */ 
#define foreach_list_safe(__node, __list)			     \
   for (exec_node * __node = (__list)->head, * __next = __node->next \
	; __next != nullptr					     \
	; __node = __next, __next = __next->next)

#define foreach_list(__node, __list)			\
   for (exec_node * __node = (__list)->head		\
	; (__node)->next != nullptr 			\
	; (__node) = (__node)->next)

#define foreach_list_const(__node, __list)		\
   for (const exec_node * __node = (__list)->head	\
	; (__node)->next != nullptr 			\
	; (__node) = (__node)->next)

#define foreach_list_typed(__type, __node, __field, __list)		\
   for (__type * __node =						\
	   exec_node_data(__type, (__list)->head, __field);		\
	(__node)->__field.next != nullptr; 				\
	(__node) = exec_node_data(__type, (__node)->__field.next, __field))

#define foreach_list_typed_const(__type, __node, __field, __list)	\
   for (const __type * __node =						\
	   exec_node_data(__type, (__list)->head, __field);		\
	(__node)->__field.next != nullptr; 				\
	(__node) = exec_node_data(__type, (__node)->__field.next, __field))

inline bool check_list_integrity(exec_list* list)
{
	foreach_iter(exec_list_iterator, iter, *list)
	{
		exec_node *node = (exec_node *)iter.get();
		if (node->check_invariants() == false)
		{
			return false;
		}
	}
	return true;
}

#endif /* LIST_CONTAINER_H */
