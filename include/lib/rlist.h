#ifndef _LIB_RLIST_H_
#define _LIB_RLIST_H_

#include "lib/spinlock.h"
#include <stddef.h>

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

#define RLIST_USE(list, node)                                                  \
  for (node = __rlist_use(list); (node) != NULL;                               \
       spinlock_release(&(node)->lock), (node) = NULL)

#define RLIST_INIT(list)                                                       \
  do {                                                                         \
    (list)->cur = NULL;                                                        \
    (list)->lock = (spinlock_t)SPINLOCK_ZERO(misc_rlist);                      \
  } while (0);

#define RLIST_CREATE(name)                                                     \
  struct rlist name = {.cur = NULL, .lock = SPINLOCK_ZERO(name)}

#endif
