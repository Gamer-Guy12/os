#ifndef _LIB_RLIST_H_
#define _LIB_RLIST_H_

#include "lib/spinlock.h"

// Ring List
struct rlist {
  struct rlist_node *cur;
};

struct rlist_node {
  struct rlist_node *next;
  spinlock_t lock;
};

void rlist_insert(struct rlist *list, struct rlist_node *node);

#endif
