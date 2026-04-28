#include "kernel/timers.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "lib/rbtree.h"
#include "lib/rlist.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

CLS(uint64_t, ticks);
CLS(struct tick_handler *, tick_handlers);
static struct gheap_cache tick_handler_cache;
static struct gheap_cache timer_handler_cache;
RLIST_CREATE(timers);

struct tick_handler {
  struct tick_handler *next;
  void (*handler)(void *);
  uint64_t ticks;
  void *data;
};

struct timer_handler {
  struct rbnode node;
  uint64_t deadline;
  void *data;
  struct timer *timer;
  void (*handler)(void *);
};

static int compare_handler(struct rbnode *n1, struct rbnode *n2) {
  struct timer_handler *t1 =
      (struct timer_handler *)((uintptr_t)n1 -
                               offsetof(struct timer_handler, node));
  struct timer_handler *t2 =
      (struct timer_handler *)((uintptr_t)n2 -
                               offsetof(struct timer_handler, node));

  if (t1->deadline > t2->deadline)
    return 1;
  else if (t1->deadline < t2->deadline)
    return -1;
  else
    return 0;
}

static void handle_timers(void) {
  struct tick_handler **handlers = GET_CLS(tick_handlers);

  while (*handlers != NULL &&
         (*handlers)->ticks == *(uint64_t *)GET_CLS(ticks)) {
    void (*handler)(void *) = (*handlers)->handler;
    void *data = (*handlers)->data;

    struct tick_handler *cur_handler = *handlers;
    *handlers = cur_handler->next;

    gheap_cache_free(&tick_handler_cache, cur_handler);

    handler(data);
  }
}

extern uint64_t hpet_read_reg(uint16_t reg);

void increment_tick(void) {
  uint64_t *this_tick = GET_CLS(ticks);
  (*this_tick)++;

  // Run anything that happens every tick
  handle_timers();
  schedule();
}

uint64_t get_cur_tick(void) {
  uint64_t *this_tick = GET_CLS(ticks);
  return *this_tick;
}

INIT void init_timers(void) {
  gheap_cache_create(&tick_handler_cache, sizeof(struct tick_handler),
                     ZONE_ANY);
  gheap_cache_create(&timer_handler_cache, sizeof(struct timer_handler),
                     ZONE_ANY);
}

void register_timer(struct timer *timer) {
  rb_create(&timer->internal.timer_handlers, compare_handler);

  rlist_insert(&timers, &timer->internal.node);
}

static void timer_handler_cb(void *passed_timer) {
  struct timer *timer = passed_timer;

  spinlock_acquire(&timer->internal.node.lock);

  // execute all the handlers that have passed
  struct timer_handler *min_handler = NULL;
  do {
    struct rbnode *min_node =
        rb_find_min(&timer->internal.timer_handlers, NULL);

    if (min_node == NULL)
      break;

    min_handler =
        (struct timer_handler *)((uintptr_t)min_node -
                                 offsetof(struct timer_handler, node));

    if (min_handler->deadline > abs_time())
      break;

    rb_delete(&timer->internal.timer_handlers, min_node);
    min_handler->handler(min_handler->data);
    gheap_cache_free(&timer_handler_cache, min_handler);
  } while (min_handler != NULL && min_handler->deadline <= abs_time());

  if (min_handler != NULL)
    timer->wait_deadline(timer_handler_cb, min_handler->timer, timer,
                         min_handler->deadline);
  spinlock_release(&timer->internal.node.lock);
}

void int_in_ms(uint64_t ms, void (*handler)(void *), void *data) {
  disable_interrupts();
  uint64_t deadline = abs_ms_deadline(ms);
  struct timer_handler *timer_handler = gheap_cache_alloc(&timer_handler_cache);

  timer_handler->handler = handler;
  timer_handler->data = data;
  timer_handler->deadline = deadline;

  struct rlist_node *node;
  RLIST_USE(&timers, node) {
    struct timer *timer =
        (struct timer *)((uintptr_t)node -
                         offsetof(struct timer, internal.node));

    timer_handler->timer = timer;
    rb_insert(&timer->internal.timer_handlers, &timer_handler->node);

    struct timer_handler *min_handler = NULL;
    do {
      struct rbnode *min_node =
          rb_find_min(&timer->internal.timer_handlers, NULL);

      if (min_node == NULL)
        break;

      min_handler =
          (struct timer_handler *)((uintptr_t)min_node -
                                   offsetof(struct timer_handler, node));

      if (min_handler->deadline > abs_time())
        break;

      rb_delete(&timer->internal.timer_handlers, min_node);
      min_handler->handler(min_handler->data);
      gheap_cache_free(&timer_handler_cache, min_handler);
    } while (min_handler != NULL && min_handler->deadline <= abs_time());

    if (min_handler != NULL) {
      timer->wait_deadline(timer_handler_cb, min_handler->timer, timer,
                           min_handler->deadline);
    }
  }
  enable_interrupts();
}

void int_at_ticks(uint64_t ticks, void (*handler)(void *), void *data) {
  struct tick_handler **this_handlers = GET_CLS(tick_handlers);
  struct tick_handler *handler_data = gheap_cache_alloc(&tick_handler_cache);

  handler_data->handler = handler;
  handler_data->ticks = ticks;
  handler_data->data = data;

  disable_interrupts();

  if (*this_handlers == NULL) {
    *this_handlers = handler_data;
    handler_data->next = NULL;
  } else if ((*this_handlers)->ticks > ticks) {
    handler_data->next = *this_handlers;
    *this_handlers = handler_data;
  } else {
    struct tick_handler *cur = *this_handlers;

    while (cur->next != NULL) {
      if (cur->next->ticks > ticks) {
        handler_data->next = cur->next;
        cur->next = handler_data;
        break;
      }

      cur = cur->next;
    }

    if (cur->next == NULL) {
      cur->next = handler_data;
      handler_data->next = NULL;
    }
  }

  enable_interrupts();
}
