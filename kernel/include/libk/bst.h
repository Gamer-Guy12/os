#ifndef BST_H
#define BST_H

#include <stdint.h>

/// Uses left duplication
///
/// Horrendously innefficient but im to lazy right now
typedef struct bst_node_struct {
  struct bst_node_struct *parent;
  struct bst_node_struct *left;
  struct bst_node_struct *right;
  uint64_t key;
} bst_node_t;

/// Finds first occurence
bst_node_t *bst_find_node(bst_node_t *root, uint64_t key);

bst_node_t *bst_insert(bst_node_t *root, bst_node_t *node);

/// Deletes first occurence
/// Does not free node, returns it for you to free
bst_node_t *bst_delete(bst_node_t *root, uint64_t key);

bst_node_t *bst_delete_node(bst_node_t *node);

void bst_create_node(bst_node_t *node, uint64_t key);

bst_node_t *bst_max(bst_node_t *tree);

bst_node_t *bst_min(bst_node_t *tree);

#endif
