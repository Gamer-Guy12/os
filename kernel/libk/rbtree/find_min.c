#include <libk/rbtree.h>
#include <libk/spinlock.h>

rbnode_t *rb_find_min(rbtree_t *tree, rbnode_t *node) {
  if (node == NULL) {
    return NULL;
  }

  spinlock_acquire(&tree->tree_lock);

  while (node->left != &tree->nil) {
    node = node->left;
  }

  spinlock_release(&tree->tree_lock);

  return node;
}
