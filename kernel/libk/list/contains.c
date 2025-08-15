#include <libk/list.h>

bool list_contains(list_t* list, list_node_t* node) {
  LIST_ITERATE(list) {
    if (cur_node == node) {
      return true;
    }
  }

  return false;
}

