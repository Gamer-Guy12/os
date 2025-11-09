#include "lib/freelist.h"
#include <stdbool.h>

// The goal is to update head->next to be node and for node->next to be what was
// in head->next
void freelist_insert(struct freelist_node *head, struct freelist_node *node) {
  node->next = __atomic_load_n(&head->next, __ATOMIC_ACQUIRE);
  while (!__atomic_compare_exchange_n(&head->next, &node->next, node, false,
                                      __ATOMIC_RELEASE, __ATOMIC_ACQUIRE))
    ;
}

// The goal is that the return value contains head->next and head->next contains
// return->next
struct freelist_node *freelist_get(struct freelist_node *head) {
  struct freelist_node *ret = __atomic_load_n(&head->next, __ATOMIC_ACQUIRE);
  while (!__atomic_compare_exchange_n(&head->next, &ret, ret->next, false,
                                      __ATOMIC_RELEASE, __ATOMIC_ACQUIRE))
    ;

  return ret;
}
