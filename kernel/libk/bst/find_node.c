#include <libk/bst.h>
#include <stddef.h>

bst_node_t *bst_find_node(bst_node_t *root, uint64_t key) {
  if (root->key == key) {
    return root;
  } else if (key < root->key) {
    if (!root->left) {
      return NULL;
    }
    return bst_find_node(root->left, key);
  } else {
    if (!root->right) {
      return NULL;
    }
    return bst_find_node(root->right, key);
  }
}
