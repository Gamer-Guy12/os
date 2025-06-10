#include <libk/bst.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Assumes that the node has a parent
bst_node_t *bst_delete_node(bst_node_t *node) {
  bool is_right = node->key > node->parent->key;

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
    // that sucker actually no need to delete just move it or not

    bst_node_t *successor = bst_max(node->left);
    node->key = successor->key;
    return bst_delete_node(successor);
  }

  return node;
}
