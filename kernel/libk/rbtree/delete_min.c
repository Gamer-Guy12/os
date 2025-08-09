#include <libk/rbtree.h>

void delete(rbtree_t *tree, rbnode_t *node);

rbnode_t* rb_delete_min(rbtree_t *tree, rbnode_t *node) {
  if (node == NULL) {
    return NULL;
  }

  spinlock_acquire(&tree->tree_lock);

  while (node->left != &tree->nil) {
    node = node->left;
  }

  delete(tree, node);

  spinlock_release(&tree->tree_lock);

  return node;
}

