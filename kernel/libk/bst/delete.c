#include <libk/bst.h>
#include <stdbool.h>
#include <stddef.h>

// The last case is ineficcient as shit
// Assumes that the node has a parent
bst_node_t *bst_delete(bst_node_t *root, uint64_t key) {
  bst_node_t *node = bst_find_node(root, key);
  return bst_delete_node(node);
}
