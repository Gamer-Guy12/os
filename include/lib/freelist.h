#ifndef _LIB_FREELIST_H_
#define _LIB_FREELIST_H_

// Also used as the list itself (like list_node)
struct freelist_node {
  struct freelist_node *next;
};

void freelist_insert(struct freelist_node *list, void *node);
void *freelist_remove(struct freelist_node *list);

#define FREELIST_CREATE(name) struct freelist_node name = {.next = &name}
#define FREELIST_INIT(freelist)                                                \
  do {                                                                         \
    (freelist)->next = (freelist);                                              \
  } while (0)

#endif
