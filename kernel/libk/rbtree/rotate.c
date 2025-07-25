#include <libk/rbtree.h>

rbnode_t *rb_rotate(rbtree_t *tree, rbnode_t *node, uint8_t dir) {
  rbnode_t* parent = node->parent;
  rbnode_t* new_root = node->child[1 - dir];
  rbnode_t* new_child = new_root->child[dir];

  node->child[dir] = new_child;

  if (new_child) new_child->parent = node;

  new_root->child[dir] = node;

  new_root->parent = parent;
  node->parent = new_root;
  if (parent) {
    parent->child[node == parent->right] = new_root;
  } else {
    tree->root = new_root;
  }

  return new_root;
}

