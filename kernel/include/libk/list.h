#ifndef LIST_H
#define LIST_H

#include <libk/spinlock.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct list_node_struct {
  struct list_node_struct *next;
  struct list_node_struct *prev;
} list_node_t;

typedef struct {
  list_node_t *head;
  list_node_t *tail;
  spinlock_t lock;
  size_t count;
} list_t;

/// input null to input at the front
void list_insert(list_t *list, list_node_t *after, list_node_t *node);
/// Input NULL to pop of the front
list_node_t *list_delete(list_t *list, list_node_t *node);

void list_create(list_t *list);

list_node_t *list_find(list_t *list, size_t index);
bool list_contains(list_t *list, list_node_t *node);

#define LIST_ITERATE(list)                                                     \
  for (list_node_t *cur_node = (list)->head; cur_node != NULL;                 \
       cur_node = cur_node->next)

#endif
