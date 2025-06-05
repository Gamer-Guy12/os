#include <libk/bst.h>
#include <stdbool.h>
#include <stddef.h>

// The last case is ineficcient as shit
bst_node_t *bst_delete(bst_node_t *root, uint64_t key) {
  bst_node_t *node = bst_find_node(root, key);
  bool is_right = key > node->parent->key;

  if (!node->left && !node->right) {
    if (is_right) {
      node->parent->right = NULL;
    } else {
      node->parent->left = NULL;
    }
  } else if (node->left && !node->right) {
    if (is_right) {
      node->parent->right = node->left;
    } else {
      node->parent->left = node->left;
    }
  } else if (!node->left && node->right) {
    if (is_right) {
      node->parent->right = node->right;
    } else {
      node->parent->left = node->right;
    }
  } else {
    // First finds its max in the left tree and then replace it and then delete
    // that sucker
  }

  return node;
}
