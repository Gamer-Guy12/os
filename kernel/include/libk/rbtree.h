/// A lot of the code in this file and its dependents is from the wikipedia page
/// for red black trees

#ifndef RBTREE_H
#define RBTREE_H

#include <libk/spinlock.h>
#include <stddef.h>
#include <stdint.h>

#define RB_BLACK 0
#define RB_RED 1

#define RB_LEFT 0
#define RB_RIGHT 1

typedef struct rbnode_struct {
  struct rbnode_struct *parent;
  union {
    struct {
      struct rbnode_struct *left;
      struct rbnode_struct *right;
    };
    struct rbnode_struct *child[2];
  };
  size_t value;
  uint8_t color;
} rbnode_t;

typedef struct {
  rbnode_t *root;
  spinlock_t tree_lock;
  rbnode_t nil;
} rbtree_t;

void rb_create(rbtree_t *tree);
void rb_insert(rbtree_t *tree, rbnode_t *node);
void rb_delete(rbtree_t *tree, rbnode_t *node);

rbnode_t *rb_search(rbtree_t *tree, rbnode_t *node, size_t value);
rbnode_t *rb_find_min(rbtree_t *tree, rbnode_t *node);
rbnode_t *rb_find_max(rbtree_t *tree, rbnode_t *node);

/// Internal do not use
rbnode_t *rb_rotate(rbtree_t *tree, rbnode_t *node, uint8_t dir);
/// Internal do not use
#define RB_DIRECTION(node) ((node) == (node)->parent->right ? RB_RIGHT : RB_LEFT)

#endif
