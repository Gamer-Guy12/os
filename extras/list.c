#include <stdbool.h>
#include <stdio.h>

// Always make sure that next pointer is consistent and then update prev
// nodes can only be inserted before nodes that are is_static
struct list_node {
  struct list_node *prev;
  struct list_node *next;
  bool is_static;
};

#define LIST_INIT(list)                                                        \
  do {                                                                         \
    (list)->prev = list;                                                       \
    (list)->next = list;                                                       \
    (list)->is_static = true;                                                  \
  } while (0)

// Inserts before list
// 0 Failure
// 1 Success
int node_insert(struct list_node *node, struct list_node *list) {
  // the head of the list ist by default static
  if (!list->is_static) {
    return 0;
  }

  return 1;
}

int main(void) {
  printf("Hello World\n");

  return 0;
}
