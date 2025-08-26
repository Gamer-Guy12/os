#include "libk/kio.h"
#include <asm.h>
#include <hpet.h>
#include <libk/list.h>
#include <libk/math.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define SLEEP(element) (sleep_t *)((size_t)(element) - offsetof(sleep_t, node))
#define WAKE_UP_PRIORITY(thread) ((thread)->priority == TP_IO ? TP_IO : TP_HIGH)

/// One of the HPETs will be used for the sleep queue
typedef struct {
  uint8_t hpet_number;
  list_t sleeps;
  mutex_t lock;
} sleep_queue_t;

typedef volatile struct {
  uint128_t tick_diff;
  list_node_t node;
  TCB_t *thread;
} sleep_t;

static sleep_queue_t sleep_queue;

static void sleep_callback(void) {
  mutex_acquire(&sleep_queue.lock);

  list_node_t* first_node = list_delete(&sleep_queue.sleeps, NULL); 
  sleep_t* first_sleep = SLEEP(first_node);

  first_sleep->thread->state = THREAD_RUNNING;
  schedule_thread(first_sleep->thread, WAKE_UP_PRIORITY(first_sleep->thread));

  mutex_release(&sleep_queue.lock);
}

void sleep_for(uint64_t ms) {
  sleep_t *sleep = gmalloc(sizeof(sleep_t));
  sleep->thread = TCB;
  uint128_t cycles = hpet_cycles_wait(ms);
  uint128_t cycle_diff = cycles;

  mutex_acquire(&sleep_queue.lock);

  if (sleep_queue.sleeps.head == NULL) {
    /// Insert in an empty queue
    sleep->tick_diff = cycle_diff;
    list_insert(&sleep_queue.sleeps, NULL, (list_node_t *)&sleep->node);
    bind_hpet_callback(sleep_callback, sleep_queue.hpet_number);
    hpet_interrupt_in_cycles(cycle_diff, sleep_queue.hpet_number);

    goto done;
  }

  bool inserted = false;

  LIST_ITERATE(&sleep_queue.sleeps, element) {
    sleep_t *cur_sleep = SLEEP(element);

    if (less_than_128(cur_sleep->tick_diff, cycle_diff)) {
      cycle_diff = sub_128(cycle_diff, cur_sleep->tick_diff);
      continue;
    }

    /// The node should be inserted before sleep
    if (cur_sleep->node.prev) {
      /// Insert in the middle
      cur_sleep->tick_diff = sub_128(cur_sleep->tick_diff, cycle_diff);
      sleep->tick_diff = cycle_diff;
      list_insert(&sleep_queue.sleeps, cur_sleep->node.prev,
                  (list_node_t *)&sleep->node);
    } else {
      /// Insert at the beginning

      sleep->tick_diff = cycle_diff;
      list_insert(&sleep_queue.sleeps, NULL, (list_node_t *)&sleep->node);
      bind_hpet_callback(sleep_callback, sleep_queue.hpet_number);
      hpet_interrupt_in_cycles(cycle_diff, sleep_queue.hpet_number);
    }

    inserted = true;
  }

  if (!inserted) {
    /// Insert at the end
    sleep->tick_diff = cycle_diff;
    list_insert(&sleep_queue.sleeps, sleep_queue.sleeps.tail,
                (list_node_t *)&sleep->node);
  }

done:

  mutex_release(&sleep_queue.lock);

  TCB->state = THREAD_WAITING;
  run_next_thread();
}

void init_sleep(void) {
  sleep_queue.hpet_number = NO_HPET;

  while (sleep_queue.hpet_number == NO_HPET) {
    sleep_queue.hpet_number = reserve_hpet();
  }

  list_create(&sleep_queue.sleeps);
  mutex_create(&sleep_queue.lock);
}
