#include <libk/bst.h>

bst_node_t *bst_min(bst_node_t *tree) {
  bst_node_t *temp = tree;

  while (temp->left) {
    temp = temp->left;
  }

  return temp;
}
