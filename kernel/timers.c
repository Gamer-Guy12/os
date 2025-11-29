#include "kernel/timers.h"
#include "kernel/kprintf.h"
#include "interrupts.h"
#include "kernel/cores.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "util.h"
#include <stdint.h>

CLS(uint64_t, ticks);
CLS(struct tick_handler *, tick_handlers);
static struct gheap_cache tick_handler_cache;

struct tick_handler {
  struct tick_handler *next;
  void (*handler)(void *);
  uint64_t ticks;
  void *data;
};

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

void increment_tick(void) {
  uint64_t *this_tick = GET_CLS(ticks);
  (*this_tick)++;

  // Run anything that happens every tick
  handle_timers();
}

uint64_t get_cur_tick(void) {
  uint64_t *this_tick = GET_CLS(ticks);
  return *this_tick;
}

INIT void init_timers(void) {
  gheap_cache_create(&tick_handler_cache, sizeof(struct tick_handler), ZONE_ANY);
}

void int_at_ticks(uint64_t ticks, void (*handler)(void *), void *data) {
  struct tick_handler **this_handlers = GET_CLS(tick_handlers);
  struct tick_handler *handler_data = gheap_cache_alloc(&tick_handler_cache);

  handler_data->handler = handler;
  handler_data->ticks = ticks;
  handler_data->data = data;

  disable_interrupts();

  if (*this_handlers == NULL) {
    kprintf("Loading\n");
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
