#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void delete(rbtree_t *tree, rbnode_t *node);

static void swap_nodes(rbtree_t *tree, rbnode_t *node1, rbnode_t *node2) {
  uint8_t dir1 = 3;
  uint8_t dir2 = 3;

  if (node1->parent)
    dir1 = RB_DIRECTION(node1);
  if (node2->parent)
    dir2 = RB_DIRECTION(node2);

  rbnode_t *temp = node1->parent;
  node1->parent = node2->parent;
  node2->parent = temp;

  if (dir1 < 2) {
    node2->parent->child[dir1] = node2;
  } else {
    tree->root = node2;
  }

  if (dir2 < 2) {
    node1->parent->child[dir2] = node1;
  } else {
    tree->root = node1;
  }

  temp = node1->left;
  node1->left = node2->left;
  node2->left = temp;

  if (node1->left != &tree->nil) {
    node1->left->parent = node1;
  }

  if (node2->left != &tree->nil) {
    node2->left->parent = node2;
  }

  temp = node1->right;
  node1->right = node2->right;
  node2->right = temp;

  if (node1->right != &tree->nil) {
    node1->right->parent = node1;
  }

  if (node2->right != &tree->nil) {
    node2->right->parent = node2;
  }

  uint8_t temp_color = node1->color;
  node1->color = node2->color;
  node2->color = temp_color;
}

static bool handle_simple(rbtree_t *tree, rbnode_t *node) {
  // Left is non nil
  if (node->right == &tree->nil && node->left != &tree->nil) {
    if (!node->parent) {
      node->left->parent = NULL;
      tree->root = node->left;

      node->left = NULL;
      node->right = NULL;
      node->parent = NULL;
      return true;
    }

    uint8_t direction = RB_DIRECTION(node);
    node->left->parent = node->parent;
    node->parent->child[direction] = node->left;

    node->left->color = RB_BLACK;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return true;
  }

  // Right is non nil
  if (node->right != &tree->nil && node->left == &tree->nil) {
    if (!node->parent) {
      node->right->parent = NULL;
      tree->root = node->right;

      node->left = NULL;
      node->right = NULL;
      node->parent = NULL;

      return true;
    }

    uint8_t direction = RB_DIRECTION(node);
    node->right->parent = node->parent;
    node->parent->child[direction] = node->right;

    node->right->color = RB_BLACK;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    return true;
  }

  // Is root and no children
  if (node == tree->root && node->right == &tree->nil &&
      node->left == &tree->nil) {
    tree->root = NULL;

    node->right = NULL;
    node->left = NULL;
    node->parent = NULL;

    return true;
  }

  // No children and red
  if (node->color == RB_RED && node->right == &tree->nil &&
      node->left == &tree->nil) {
    uint8_t direction = RB_DIRECTION(node);

    node->parent->child[direction] = &tree->nil;

    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    return true;
  }

  // Two children
  if (node->right != &tree->nil && node->left != &tree->nil) {
    // Find in order sucessor
    rbnode_t *successor = node->right;
    while (successor->left != &tree->nil) {
      successor = successor->left;
    }

    swap_nodes(tree, successor, node);

    delete(tree, node);

    return true;
  }

  return false;
}

void delete(rbtree_t *tree, rbnode_t *node) {
  if (handle_simple(tree, node)) {
    return;
  }

  rbnode_t *parent = node->parent;

  rbnode_t *sibling;
  rbnode_t *close_nephew;
  rbnode_t *distant_nephew;

  uint8_t dir = RB_DIRECTION(node);
  parent->child[dir] = &tree->nil;
  goto start_balance;

  do {
    dir = RB_DIRECTION(node);

  start_balance:
    sibling = parent->child[1 - dir];
    distant_nephew = sibling->child[1 - dir];
    close_nephew = sibling->child[dir];

    if (sibling->color == RB_RED) {
      rb_rotate(tree, parent, dir);
      parent->color = RB_RED;
      sibling->color = RB_BLACK;
      sibling = close_nephew;

      distant_nephew = sibling->child[1 - dir];
      if (distant_nephew && distant_nephew->color == RB_RED) {
        goto case_6;
      }
      close_nephew = sibling->child[dir];
      if (close_nephew && close_nephew->color == RB_RED) {
        goto case_5;
      }

      sibling->color = RB_RED;
      parent->color = RB_BLACK;
      return;
    }

    if (distant_nephew && distant_nephew->color == RB_RED) {
      goto case_6;
    }

    if (close_nephew && close_nephew->color == RB_RED) {
      goto case_5;
    }

    if (parent->color == RB_RED) {
      sibling->color = RB_RED;
      parent->color = RB_BLACK;
      return;
    }

    if (!parent)
      return;

    sibling->color = RB_RED;
    node = parent;
  } while ((parent = node->parent));

  if (parent == NULL)
    return;

case_5:
  rb_rotate(tree, sibling, 1 - dir);
  sibling->color = RB_RED;
  close_nephew->color = RB_BLACK;
  distant_nephew = sibling;
  sibling = close_nephew;

case_6:
  rb_rotate(tree, parent, dir);
  sibling->color = parent->color;
  parent->color = RB_BLACK;
  distant_nephew->color = RB_BLACK;

  return;
}

void rb_delete(rbtree_t *tree, rbnode_t *node) {
  spinlock_acquire(&tree->tree_lock);

  delete(tree, node);

  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;

  spinlock_release(&tree->tree_lock);

  __atomic_fetch_sub(&tree->count, 1, __ATOMIC_RELEASE);
}
