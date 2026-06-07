#ifndef _LIB_FREELIST_H_
#define _LIB_FREELIST_H_

#include <stddef.h>

// Also used as the list itself (like list_node)
struct freelist_node {
  struct freelist_node *next;
  size_t count;
};

void freelist_insert(struct freelist_node *list, void *node);
void *freelist_remove(struct freelist_node *list);

#define FREELIST_CREATE(name)                                                  \
  struct freelist_node name = {.next = &name, .count = 0}
#define FREELIST_INIT(freelist)                                                \
  do {                                                                         \
    (freelist)->next = (freelist);                                             \
    (freelist)->count = 0;                                                     \
  } while (0)

#endif
