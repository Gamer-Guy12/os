#include <libk/bst.h>

bst_node_t *bst_max(bst_node_t *tree) {
  bst_node_t *temp = tree;

  while (temp->right) {
    temp = temp->right;
  }

  return temp;
}
