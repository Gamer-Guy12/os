#ifndef _LIB_FREELIST_H_
#define _LIB_FREELIST_H_

// The first node in the freelist is the head
struct freelist_node {
  struct freelist_node *next;
};

void freelist_insert(struct freelist_node *head, struct freelist_node *node);
struct freelist_node *freelist_get(struct freelist_node *head);

#endif
