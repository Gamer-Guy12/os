#include <libk/rbtree.h>
#include <libk/spinlock.h>

static void insert(rbtree_t* tree, rbnode_t* node) {
  if (tree->root == NULL) {
    tree->root = node;
  }

  node->left = &tree->nil;
  node->right = &tree->nil;
  node->parent = NULL;
  node->color = RB_BLACK;
}

void rb_insert(rbtree_t *tree, rbnode_t *node) {
  if (node == NULL) return;
  
  spinlock_acquire(&tree->tree_lock);

  if (tree->root == NULL) {
    insert(tree, node);
    return;
  }

  insert(tree, node);

  spinlock_release(&tree->tree_lock);
}

