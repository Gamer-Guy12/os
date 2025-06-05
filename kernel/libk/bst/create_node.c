#include <libk/bst.h>
#include <stddef.h>

void bst_create_node(bst_node_t *node, uint64_t key) {
  node->key = key;
  node->parent = NULL;
  node->right = NULL;
  node->left = NULL;
}
