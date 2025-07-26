#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void insert(rbtree_t *tree, rbnode_t *node) {
  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = NULL;
  node->color = RB_RED;

  if (tree->root == NULL) {
    tree->root = node;
    return;
  }

  rbnode_t *cur_node = tree->root;

  while (true) {
    if (node->value > cur_node->value) {
      rbnode_t *parent = cur_node;
      cur_node = cur_node->right;

      if (cur_node == &tree->nil) {
        parent->right = node;
        node->parent = parent;
        return;
      }
    } else {
      rbnode_t *parent = cur_node;
      cur_node = cur_node->left;

      if (cur_node == &tree->nil) {
        parent->left = node;
        node->parent = parent;
        return;
      }
    }
  }
}

void handle_insert(rbtree_t *tree, rbnode_t *node) {
  rbnode_t *parent = node->parent;
  uint8_t dir = 0;

  node->color = RB_RED;

  if (!parent) {
    return;
  }

  do {
    if (parent->color == RB_BLACK) {
      return;
    }

    rbnode_t *grandparent = parent->parent;

    if (!grandparent) {
      parent->color = RB_BLACK;
      return;
    }

    dir = RB_DIRECTION(parent);
    rbnode_t *uncle = grandparent->child[1 - dir];
    if (uncle == &tree->nil || uncle->color == RB_BLACK) {
      if (node == parent->child[1 - dir]) {
        rb_rotate(tree, parent, dir);
        node = parent;
        parent = grandparent->child[dir];
      }

      rb_rotate(tree, grandparent, 1 - dir);
      parent->color = RB_BLACK;
      grandparent->color = RB_RED;
      return;
    }

    parent->color = RB_BLACK;
    uncle->color = RB_BLACK;
    grandparent->color = RB_RED;
    node = grandparent;
  } while (parent);
}

void rb_insert(rbtree_t *tree, rbnode_t *node) {
  if (node == NULL)
    return;

  spinlock_acquire(&tree->tree_lock);

  if (tree->root == NULL) {
    insert(tree, node);
    spinlock_release(&tree->tree_lock);
    return;
  }

  insert(tree, node);

  handle_insert(tree, node);

  spinlock_release(&tree->tree_lock);
}
