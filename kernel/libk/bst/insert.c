#include <libk/bst.h>

bst_node_t *bst_insert(bst_node_t *root, bst_node_t *node) {
  if (node->key > root->key) {
    if (!root->right) {
      root->right = node;
      return node;
    } else {
      return bst_insert(root->right, node);
    }
  } else {
    if (!root->left) {
      root->left = node;
      return node;
    } else {
      return bst_insert(root->left, node);
    }
  }
}
