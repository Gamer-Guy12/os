#include <libk/rbtree.h>
#include <libk/spinlock.h>
#include <stdatomic.h>

void rb_create(rbtree_t *tree) {
  tree->root = NULL;
  atomic_flag_clear_explicit(&tree->tree_lock, memory_order_release);

  tree->nil.color = RB_BLACK;
  tree->nil.value = 0;
  tree->nil.left = NULL;
  tree->nil.right = NULL;
  tree->nil.parent = NULL;
}
