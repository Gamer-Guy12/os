#include "lib/rlist.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stddef.h>

void rlist_insert(struct rlist *list, struct rlist_node *node) {
  spinlock_acquire(&list->lock);

  if (list->cur != NULL) {
    node->next = list->cur->next;
    list->cur->next = node;
  } else {
    node->next = node;
    list->cur = node;
  }

  spinlock_release(&list->lock);
}

void rlist_cycle(struct rlist *list) {
  spinlock_acquire(&list->lock);

  list->cur = list->cur->next;

  spinlock_release(&list->lock);
}

struct rlist_node *__rlist_use(struct rlist *list) {
  spinlock_acquire(&list->lock);

  struct rlist_node *node = NULL;

  while (true) {
    node = list->cur;
    list->cur = list->cur->next;

    if (spinlock_attempt(&node->lock)) {
      break;
    }
  }

  spinlock_release(&list->lock);

  return node;
}
