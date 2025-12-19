#ifndef _LIB_RLIST_H_
#define _LIB_RLIST_H_

#include "lib/spinlock.h"

// Ring List
struct rlist {
  struct rlist_node *cur;
  spinlock_t lock;
};

struct rlist_node {
  struct rlist_node *next;
  spinlock_t lock;
};

void rlist_insert(struct rlist *list, struct rlist_node *node);
void rlist_cycle(struct rlist *list);
struct rlist_node *__rlist_use(struct rlist *list);

#define RLIST_USE(list, node) for (node = __rlist_use(list); 1 < 0; spinlock_release(&node->lock))

#endif
