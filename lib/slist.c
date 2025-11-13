#include "lib/slist.h"
#include <stdbool.h>
#include <stddef.h>

void slist_push(struct slist_node *list, struct slist_node *node) {
  node->next = __atomic_load_n(&list->next, __ATOMIC_ACQUIRE);
  while (!__atomic_compare_exchange_n(&list->next, &node->next, node, false,
                                      __ATOMIC_RELEASE, __ATOMIC_ACQUIRE))
    ;
}

struct slist_node *slist_pop(struct slist_node *list) {
  struct slist_node *ret = __atomic_load_n(&list->next, __ATOMIC_ACQUIRE);
  while (!__atomic_compare_exchange_n(&list->next, &ret, ret->next, false,
                                      __ATOMIC_RELEASE, __ATOMIC_ACQUIRE))
    ;
  if (ret == list)
    return NULL;

  return ret;
}
