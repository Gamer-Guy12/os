#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <stdint.h>

static void insert(rbtree_t* tree, rbnode_t* node) {
  if (tree->root == NULL) {
    tree->root = node;
  }

  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = NULL;
  node->color = RB_BLACK;
}

void handle_insert(rbtree_t* tree, rbnode_t* node) {
  rbnode_t* parent = node->parent;
  uint8_t dir = 0;

  node->color = RB_RED;

  if (!parent) {
    return;
  }
  
  do {
    if (parent->color == RB_BLACK) return;

    parent = node->parent;

    rbnode_t* grandparent = parent->parent;

    if (!grandparent) {
      parent->color = RB_BLACK;
      return;
    }

    dir = RB_DIRECTION(parent);
    rbnode_t* uncle = grandparent->child[1 - dir];
    if (uncle == &tree->nil || uncle->color == RB_BLACK) {
      if (node == parent->child[1 - dir]) {
        rb_rotate(tree, parent, dir);
        node = parent;
        parent = grandparent->child[dir];
      }

      rb_rotate(tree, grandparent,  1 - dir);
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
  if (node == NULL) return;
  
  spinlock_acquire(&tree->tree_lock);

  if (tree->root == NULL) {
    insert(tree, node);
    return;
  }

  insert(tree, node);

  handle_insert(tree, node);

  spinlock_release(&tree->tree_lock);
}

