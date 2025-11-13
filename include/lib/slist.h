// Singly-Linked List

#ifndef _LIB_SLIST_H_
#define _LIB_SLIST_H_

struct slist_node {
  struct slist_node *next;
};

void slist_push(struct slist_node *list, struct slist_node *node);
struct slist_node *slist_pop(struct slist_node *list);

#define SlIST_CREATE(name) struct slist_node name = {.next = &name}
#define SLIST_INIT(list)                                                       \
  do {                                                                         \
    (list)->next = list;                                                       \
  } while (0)

#define SLIST_FOREACH(pos, list)                                               \
  for ((pos) = (list)->next; (pos) != (list); (pos) = (pos)->next)

#endif
