#include <libk/rbtree.h>
#include <libk/spinlock.h>

rbnode_t *rb_find_max(rbtree_t *tree, rbnode_t *node) {
  if (node == NULL) {
    return NULL;
  }

  spinlock_acquire(&tree->tree_lock);

  while (node->right != &tree->nil) {
    node = node->right;
  }

  spinlock_release(&tree->tree_lock);

  return node;
}
