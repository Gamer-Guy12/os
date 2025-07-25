#include <libk/rbtree.h>
#include <stdbool.h>

rbnode_t *rb_search(rbtree_t *tree, rbnode_t *node, size_t value) {
  if (node == NULL) {
    node = tree->root;
  }

  if (node == NULL) {
    return NULL;
  }

  while (true) {
    if (node->value > value) {
      node = node->left;

      if (node == &tree->nil) {
        return NULL;
      }
    } else if (node->value < value) {
      node = node->right;

      if (node == &tree->nil) {
        return NULL;
      }
    } else {
      return node;
    }
  }
}
