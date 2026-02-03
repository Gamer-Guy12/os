#include "lib/rbtree.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void __rb_delete(struct rbtree *tree, struct rbnode *node);

void rb_create(struct rbtree *tree,
               int (*compare)(struct rbnode *n1, struct rbnode *n2)) {
  tree->compare = compare;
  tree->root = NULL;
  tree->count = 0;
  tree->lock = (spinlock_t)SPINLOCK_ZERO;
}

static void insert(struct rbtree *tree, struct rbnode *node) {
  node->left = &rbnil;
  node->right = &rbnil;
  node->parent = NULL;
  node->color = RB_RED;

  if (tree->root == NULL) {
    tree->root = node;
    return;
  }

  struct rbnode *cur = tree->root;

  while (true) {
    if (tree->compare(node, cur) > 0) {
      struct rbnode *parent = cur;
      cur = cur->right;

      if (cur == &rbnil) {
        parent->right = node;
        node->parent = parent;
        return;
      }
    } else {
      struct rbnode *parent = cur;
      cur = cur->left;

      if (cur == &rbnil) {
        parent->left = node;
        node->parent = parent;
        return;
      }
    }
  }
}

static void handle_insert(struct rbtree *tree, struct rbnode *node) {
  struct rbnode *parent = node->parent;
  uint8_t dir = 0;

  node->color = RB_RED;

  if (!parent) {
    return;
  }

  do {
    if (parent->color == RB_BLACK) {
      return;
    }

    struct rbnode *grandparent = parent->parent;

    if (!grandparent) {
      parent->color = RB_BLACK;
      return;
    }

    dir = __RB_DIRECTION(parent);
    struct rbnode *uncle = grandparent->child[1 - dir];
    if (uncle == &rbnil || uncle->color == RB_BLACK) {
      if (node == parent->child[1 - dir]) {
        __rb_rotate(tree, parent, dir);
        node = parent;
        parent = grandparent->child[dir];
      }

      __rb_rotate(tree, grandparent, 1 - dir);
      parent->color = RB_BLACK;
      grandparent->color = RB_RED;
      return;
    }

    parent->color = RB_BLACK;
    uncle->color = RB_BLACK;
    grandparent->color = RB_RED;
    node = grandparent;

    parent = node->parent;
  } while (parent);
}

static void __rb_insert(struct rbtree *tree, struct rbnode *node) {
  if (tree->root == NULL) {
    insert(tree, node);
    tree->count++;
    return;
  }

  insert(tree, node);
  handle_insert(tree, node);
  tree->count++;
}

void rb_insert(struct rbtree *tree, struct rbnode *node) {
  if (node == NULL)
    return;

  spinlock_acquire(&tree->lock);
  __rb_insert(tree, node);
  spinlock_release(&tree->lock);
}

static void swap_nodes(struct rbtree *tree, struct rbnode *n1,
                       struct rbnode *n2) {
  // RB_LEFT is 0, RB_RIGHT is 1, so 3 avoids both of them
  uint8_t dir1 = 3;
  uint8_t dir2 = 3;

  if (n1->parent)
    dir1 = __RB_DIRECTION(n1);
  if (n2->parent)
    dir2 = __RB_DIRECTION(n2);

  struct rbnode *temp = n1->parent;
  n1->parent = n2->parent;
  n2->parent = temp;

  if (dir1 < 2) {
    n2->parent->child[dir1] = n2;
  } else {
    tree->root = n2;
  }

  if (dir2 < 2) {
    n1->parent->child[dir2] = n1;
  } else {
    tree->root = n1;
  }

  temp = n1->left;
  n1->left = n2->left;
  n2->left = temp;

  if (n1->left != &rbnil) {
    n1->left->parent = n1;
  }

  if (n2->left != &rbnil) {
    n2->left->parent = n2;
  }

  temp = n1->right;
  n1->right = n2->right;
  n2->right = temp;

  if (n1->right != &rbnil) {
    n1->right->parent = n1;
  }

  if (n2->right != &rbnil) {
    n2->right->parent = n1;
  }

  uint8_t temp_color = n1->color;
  n1->color = n2->color;
  n2->color = temp_color;
}

// Returns true if it was fully handled
static bool handle_simple(struct rbtree *tree, struct rbnode *node) {
  // Left is non nil
  if (node->right == &rbnil && node->left != &rbnil) {
    if (!node->parent) {
      node->left->parent = NULL;
      tree->root = node->left;

      node->left = NULL;
      node->right = NULL;
      node->parent = NULL;

      tree->count--;
      return true;
    }

    uint8_t dir = __RB_DIRECTION(node);
    node->left->parent = node->parent;
    node->parent->child[dir] = node->left;

    node->left->color = RB_BLACK;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    tree->count--;
    return true;
  }

  // Right is non nil
  if (node->right != &rbnil && node->left == &rbnil) {
    if (!node->parent) {
      node->right->parent = NULL;
      tree->root = node->right;

      node->left = NULL;
      node->right = NULL;
      node->parent = NULL;

      tree->count--;
      return true;
    }

    uint8_t dir = __RB_DIRECTION(node);
    node->right->parent = node->parent;
    node->parent->child[dir] = node->right;

    node->right->color = RB_BLACK;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    tree->count--;
    return true;
  }

  // Is root and no children
  if (node == tree->root && node->right == &rbnil && node->left == &rbnil) {
    tree->root = NULL;

    node->right = NULL;
    node->left = NULL;
    node->parent = NULL;

    tree->count--;
    return true;
  }

  // No children and red
  if (node->color == RB_RED && node->left == &rbnil && node->right == &rbnil) {
    uint8_t dir = __RB_DIRECTION(node);

    node->parent->child[dir] = &rbnil;

    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;

    tree->count--;
    return true;
  }

  // Two children
  if (node->right != &rbnil && node->left != &rbnil) {
    // Find in order successor
    struct rbnode *successor = node->right;
    while (successor->left != &rbnil) {
      successor = successor->left;
    }

    swap_nodes(tree, successor, node);
    __rb_delete(tree, node);
    return true;
  }

  return false;
}

void __rb_delete(struct rbtree *tree, struct rbnode *node) {
  if (handle_simple(tree, node)) {
    return;
  }

  struct rbnode *parent = node->parent;

  struct rbnode *sibling;
  struct rbnode *close_nephew;
  struct rbnode *distant_nephew;

  uint8_t dir = __RB_DIRECTION(node);
  parent->child[dir] = &rbnil;
  goto start_balance;

  do {
    dir = __RB_DIRECTION(node);

  start_balance:
    sibling = parent->child[1 - dir];
    distant_nephew = sibling->child[1 - dir];
    close_nephew = sibling->child[dir];

    if (sibling->color == RB_RED) {
      __rb_rotate(tree, parent, dir);
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
      tree->count--;
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
      tree->count--;
      return;
    }

    if (!parent) {
      tree->count--;
      return;
    }

    sibling->color = RB_RED;
    node = parent;
  } while ((parent = node->parent));

  if (parent == NULL) {
    tree->count--;
    return;
  }

case_5:
  __rb_rotate(tree, sibling, 1 - dir);
  sibling->color = RB_RED;
  close_nephew->color = RB_BLACK;
  distant_nephew = sibling;
  sibling = close_nephew;

case_6:
  __rb_rotate(tree, parent, dir);
  sibling->color = parent->color;
  parent->color = RB_BLACK;
  distant_nephew->color = RB_BLACK;

  tree->count--;
  return;
}

void rb_delete(struct rbtree *tree, struct rbnode *node) {
  if (node == NULL)
    return;

  spinlock_acquire(&tree->lock);
  __rb_delete(tree, node);
  spinlock_release(&tree->lock);

  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;
}

// Pass in the node you want to start from for subtree
// Passing in NULL defaults to the root
struct rbnode *rb_search(struct rbtree *tree, struct rbnode *subtree,
                         struct rbnode *target) {
  spinlock_acquire(&tree->lock);

  if (subtree == NULL) {
    subtree = tree->root;
  }

  if (subtree == NULL) {
    return NULL;
  }

  while (true) {
    int compval = tree->compare(subtree, target);

    if (compval > 0) {
      subtree = subtree->left;

      if (subtree == &rbnil) {
        return NULL;
      }
    } else if (compval < 0) {
      subtree = subtree->right;

      if (subtree == &rbnil) {
        return NULL;
      }
    } else {
      spinlock_release(&tree->lock);
      return subtree;
    }
  }
}

struct rbnode *rb_find_min(struct rbtree *tree, struct rbnode *subtree) {
  if (subtree == NULL)
    subtree = tree->root;
  if (subtree == NULL)
    return NULL;

  spinlock_acquire(&tree->lock);

  while (subtree->left != &rbnil) {
    subtree = subtree->left;
  }

  spinlock_release(&tree->lock);

  return subtree;
}

struct rbnode *rb_find_max(struct rbtree *tree, struct rbnode *subtree) {
  if (subtree == NULL)
    subtree = tree->root;
  if (subtree == NULL)
    return NULL;

  spinlock_acquire(&tree->lock);

  while (subtree->right != &rbnil) {
    subtree = subtree->right;
  }

  spinlock_release(&tree->lock);

  return subtree;
}

struct rbnode *rb_delete_search(struct rbtree *tree, struct rbnode *subtree,
                                struct rbnode *target) {
  spinlock_acquire(&tree->lock);

  if (subtree == NULL) {
    subtree = tree->root;
  }

  if (subtree == NULL) {
    return NULL;
  }

  while (true) {
    int compval = tree->compare(subtree, target);

    if (compval > 0) {
      subtree = subtree->left;

      if (subtree == &rbnil) {
        return NULL;
      }
    } else if (compval < 0) {
      subtree = subtree->right;

      if (subtree == &rbnil) {
        return NULL;
      }
    } else {
      __rb_delete(tree, subtree);
      spinlock_release(&tree->lock);
      return subtree;
    }
  }
}

struct rbnode *rb_delete_min(struct rbtree *tree, struct rbnode *subtree) {
  if (subtree == NULL)
    subtree = tree->root;
  if (subtree == NULL)
    return NULL;

  spinlock_acquire(&tree->lock);

  while (subtree->left != &rbnil) {
    subtree = subtree->left;
  }

  __rb_delete(tree, subtree);
  spinlock_release(&tree->lock);

  return subtree;
}

struct rbnode *rb_delete_max(struct rbtree *tree, struct rbnode *subtree) {
  if (subtree == NULL)
    subtree = tree->root;
  if (subtree == NULL)
    return NULL;

  spinlock_acquire(&tree->lock);

  while (subtree->right != &rbnil) {
    subtree = subtree->right;
  }

  __rb_delete(tree, subtree);
  spinlock_release(&tree->lock);

  return subtree;
}

struct rbnode *__rb_rotate(struct rbtree *tree, struct rbnode *node,
                           uint8_t dir) {
  struct rbnode *parent = node->parent;
  struct rbnode *new_root = node->child[1 - dir];
  struct rbnode *new_child = new_root->child[dir];

  node->child[1 - dir] = new_child;

  if (new_child)
    new_child->parent = node;
  new_root->child[dir] = node;

  new_root->parent = parent;
  node->parent = new_root;

  if (parent) {
    parent->child[node == parent->right] = new_root;
  } else {
    tree->root = new_root;
  }

  return new_root;
}
