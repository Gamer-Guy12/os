// A freelist of memory addresses
// Essentially a singly linked list with no data

#ifndef _LIB_FREELIST_H_
#define _LIB_FREELIST_H_

// The first node in the freelist is the head
struct freelist_node {
  struct freelist_node *next;
};

void freelist_insert(struct freelist_node *head, void *node);
void *freelist_get(struct freelist_node *head);

#define FREELIST_CREATE(name) struct freelist_node name = {.next = &name}
#define FREELIST_INIT(freelist)                                                \
  do {                                                                         \
    (freelist)->next = (freelist);                                             \
  } while (0);

#endif
