#include "lib/list.h"
#include <stddef.h>

void list_insert(struct list_node *list, struct list_node *node) {
  node->next = list->next;
  node->next->prev = node;

  node->prev = list;
  list->next = node;
}

void list_remove(struct list_node *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;

  node->prev = NULL;
  node->next = NULL;
}

void *list_pop_front(struct list_node *list) {
  if (list->next == NULL)
    return NULL;

  struct list_node *ret = list->next;
  list_remove(ret);

  return ret;
}

void *list_pop_back(struct list_node *list) {
  if (list->prev == NULL)
    return NULL;

  struct list_node *ret = list->prev;
  list_remove(ret);

  return ret;
}
