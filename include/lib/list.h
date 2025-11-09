#ifndef _LIB_LIST_H_
#define _LIB_LIST_H_

struct list_node {
  struct list_node *prev;
  struct list_node *next;
};

void list_insert(struct list_node *list, struct list_node *node);
void list_remove(struct list_node *list, struct list_node *node);

#define LIST_CREATE(name) struct list_node list = {.prev = &list, .next = &list}
#define LIST_INIT(list)                                                        \
  do {                                                                         \
    (list)->prev = list;                                                       \
    (list)->next = list;                                                       \
  } while (0)

#define LIST_FOREACH(pos, list)                                                \
  for ((pos) = (list)->next; (pos) != (list); (pos) = (pos)->next)

#endif
