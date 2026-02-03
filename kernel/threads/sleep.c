#include "kernel/threads.h"
#include "kernel/timers.h"
#include "lib/list.h"
#include <stdint.h>

static struct wait_queue sleep_queue;

void init_sleep(void) { waitqueue_create(&sleep_queue); }

void sleep_wakeup(void *tid_data) {
  int tid = (int)(uintptr_t)tid_data;
  struct list_node *node = NULL;
  bool found = false;

  LIST_FOREACH(node, &sleep_queue.list) {
    struct wait_queue_node *wait_node = WQ_NODE(node);

    if (wait_node->thread->tid == tid) {
      waitqueue_awaken(&sleep_queue, wait_node);
      found = true;
      break;
    }
  }

  // Wait another ms if it didn't find it
  if (!found) {
    int_in_ms(1, sleep_wakeup, (void *)(uintptr_t)tid);
  }
}

void sleep(uint32_t ms) {
  int_in_ms(ms, sleep_wakeup, (void *)(uintptr_t)get_cur_thread()->tid);

  waitqueue_wait(&sleep_queue);
}
