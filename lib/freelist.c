#include "lib/freelist.h"
#include <stdbool.h>
#include <stddef.h>

// The goal is to update head->next to be node and for node->next to be what was
// in head->next
void freelist_insert(struct freelist_node *head, void *node) {
  struct freelist_node *fnode = node;
  do {
    fnode->next = __atomic_load_n(&head->next, __ATOMIC_ACQUIRE);
  } while (!__atomic_compare_exchange_n(&head->next, &fnode->next, fnode, false,
                                        __ATOMIC_RELEASE, __ATOMIC_ACQUIRE));
}

// The goal is that the return value contains head->next and head->next contains
// return->next
void *freelist_get(struct freelist_node *head) {
  struct freelist_node *ret;
  do {
    ret = __atomic_load_n(&head->next, __ATOMIC_ACQUIRE);
    if (ret == head)
      return NULL;
  } while (!__atomic_compare_exchange_n(&head->next, &ret, ret->next, false,
                                        __ATOMIC_RELEASE, __ATOMIC_ACQUIRE));

  return ret;
}
