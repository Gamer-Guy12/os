#ifndef _LIB_RBTREE_H_
#define _LIB_RBTREE_H_

#include "lib/spinlock.h"
#include <stddef.h>
#include <stdint.h>

#define RB_BLACK 0
#define RB_RED 1
#define RB_LEFT 0
#define RB_RIGHT 1

struct rbnode {
  struct rbnode *parent;
  union {
    struct {
      struct rbnode *left;
      struct rbnode *right;
    };
    struct rbnode *child[2];
  };
  uint8_t color;
#if __SIZEOF_POINTER__ == 8
  uint8_t padding[7];
#elif __SIZEOF_POINTER__ == 4
  uint8_t padding[3];
#else
  uin8_t padding[3];
#endif
};

__attribute__((unused)) static struct rbnode rbnil = {0};

struct rbtree {
  // Return < 0 if n1 is less than n2
  // Return 0 if they are equal
  // Return > 0 if n1 is greater than n2
  int (*compare)(struct rbnode *n1, struct rbnode *n2);
  struct rbnode *root;
  size_t count;
  spinlock_t lock;
};

void rb_create(struct rbtree *tree,
               int (*compare)(struct rbnode *n1, struct rbnode *n2));
void rb_insert(struct rbtree *tree, struct rbnode *node);
void rb_delete(struct rbtree *tree, struct rbnode *node);

// Pass in the node you want to start from for subtree
// Passing in NULL defaults to the root
//
// For target make sure that when compared with the actual node it pops 0 out of
// compare
struct rbnode *rb_search(struct rbtree *tree, struct rbnode *subtree,
                         struct rbnode *target);
struct rbnode *rb_find_min(struct rbtree *tree, struct rbnode *subtree);
struct rbnode *rb_find_max(struct rbtree *tree, struct rbnode *subtree);

struct rbnode *rb_delete_search(struct rbtree *tree, struct rbnode *subtree,
                                struct rbnode *target);
struct rbnode *rb_delete_min(struct rbtree *tree, struct rbnode *subtree);
struct rbnode *rb_delete_max(struct rbtree *tree, struct rbnode *subtree);

struct rbnode *__rb_rotate(struct rbtree *tree, struct rbnode *node,
                           uint8_t dir);
#define __RB_DIRECTION(node)                                                   \
  ((node) == (node)->parent->right ? RB_RIGHT : RB_LEFT)

#endif
