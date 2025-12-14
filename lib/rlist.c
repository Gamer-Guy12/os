#include "lib/rlist.h"
#include <stddef.h>

void rlist_insert(struct rlist *list, struct rlist_node *node) {
  while (true) {
    struct rlist_node *cur = __atomic_load_n(&list->cur, __ATOMIC_ACQUIRE); 

    if (cur == NULL) {
      __atomic_compare_exchange_n(&list->cur, &cur, node, false, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
    } else {
      node->next = cur;

      if (__atomic_compare_exchange_n(&list->cur, &cur, node, false, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE)) {
        break;
      }
    }
  }
}

