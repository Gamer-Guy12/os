#include "interrupts.h"
#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "lib/atomic.h"
#include "lib/queue.h"
#include "lib/spinlock.h"
#include <stdint.h>

#define ROUND_UP(value, to) ((((value) + ((to) - 1)) / (to)) * (to))

static struct gheap_cache task_cache;
static atomic_t cache_init = ATOMIC_ZERO;

// 2 bits
// bit 0: 0 means continue, 1 means stop
// bit 1: 1 means accept, 0 means don't
static int check_thread(struct thread *thread, void *_) { return 3; }

// Check if the queue needs to be destroyed
static void __handle_death(struct work_queue *queue) {
  spinlock_acquire(&queue->queue_lock);

  if (!queue->state) {
    spinlock_release(&queue->queue_lock);
    return;
  }

  queue->worker_threads--;
  if (queue->worker_threads != 0) {
    goto die;
  }

  struct queue_node *node = NULL;
  while ((node = queue_dequeue(&queue->tasks))) {
    struct work_task *task =
        (struct work_task *)((uintptr_t)node -
                             offsetof(struct work_task, node));

    gheap_cache_free(&task_cache, task);
  }

  queue->state = 2;

die:
  spinlock_release(&queue->queue_lock);
  terminate(0);
}

static void __handle_task(struct work_queue *queue) {
  void *data = NULL;
  void (*task_func)(void *) = NULL;

  spinlock_acquire(&queue->queue_lock);
  struct queue_node *node = queue_dequeue(&queue->tasks);
  if (node == NULL) {
    spinlock_release(&queue->queue_lock);
    return;
  }
  queue->task_count--;
  spinlock_release(&queue->queue_lock);

  struct work_task *work_node =
      (struct work_task *)((uintptr_t)node - offsetof(struct work_task, node));
  task_func = work_node->task;
  data = work_node->data;
  kprintf("%p\n", work_node);
  gheap_cache_free(&task_cache, work_node);
  while (1) {
  }

  if (task_func)
    task_func(data);
}

// Entry-point for all worker threads
static void __wait_entry(void) {
  disable_interrupts();
  struct thread *thread = get_cur_thread();
  struct work_queue *queue = thread->data;
  thread->priority = queue->priority;
  enable_interrupts();

  while (true) {
    __handle_task(queue);
    __handle_death(queue);
  }
}

static void __create_worker(struct work_queue *queue) {
  struct thread *thread = create_thread(__wait_entry, TP_NO_QUEUE);
  thread->data = queue;
  thread->priority = TP_HIGH;
  schedule_thread(thread);
  queue->worker_threads++;
}

// Wakes up workers as needed and starts tasks going
static void __run_queue(struct work_queue *queue) {
  spinlock_acquire(&queue->queue_lock);

  const size_t needed_threads =
      ROUND_UP(queue->task_count, queue->tasks_per_thread) /
      queue->tasks_per_thread;

  // Create workers as needed
  // Workers don't get destroyed because it is assumed that if x workers were
  // needed now, they will be needed sometime in the future
  while (needed_threads > queue->worker_threads) {
    __create_worker(queue);
  }

  const size_t running_threads =
      queue->worker_threads - queue->workers.wait_count.num;
  const size_t awaken_count = needed_threads - running_threads;
  for (size_t i = 0; i < awaken_count; i++) {
    waitqueue_awaken(&queue->workers, NULL);
  }

  spinlock_release(&queue->queue_lock);
}

void work_queue_create(struct work_queue *queue, enum thread_priority priority,
                       size_t tasks_per_thread) {
  if (atomic_cas(&cache_init, 0, 1)) {
    gheap_cache_create(&task_cache, sizeof(struct work_task), ZONE_ANY);
    atomic_add(&cache_init, 1);
  }

  // Wait till the cache is created
  while (atomic_load(&cache_init) != 2)
    ;

  queue->queue_lock = (spinlock_t)SPINLOCK_ZERO(misc_wait_queue);
  spinlock_acquire(&queue->queue_lock);
  queue->priority = priority;
  queue->state = 0;
  queue->worker_threads = 0;
  queue->task_count = 0;
  queue->tasks_per_thread =
      tasks_per_thread == 0 ? DEF_TASKS_THREAD : tasks_per_thread;
  QUEUE_INIT(&queue->tasks);
  waitqueue_create(&queue->workers, check_thread);

  __create_worker(queue);
  spinlock_release(&queue->queue_lock);
}

void work_queue_add(struct work_queue *queue, void (*task)(void *),
                    void *data) {
  struct work_task *work_task = gheap_cache_alloc(&task_cache);

  work_task->data = data;
  work_task->task = task;

  spinlock_acquire(&queue->queue_lock);
  queue_enqueue(&queue->tasks, &work_task->node);
  queue->task_count++;
  spinlock_release(&queue->queue_lock);

  __run_queue(queue);
}

void work_queue_destroy(struct work_queue *queue) {
  spinlock_acquire(&queue->queue_lock);
  queue->state = 1;
  waitqueue_awaken_all(&queue->workers);
  spinlock_release(&queue->queue_lock);
}

bool work_queue_dead(struct work_queue *queue) {
  bool dead = false;
  // spinlock_acquire(&queue->queue_lock);
  if (queue->state == 2)
    dead = true;
  // spinlock_release(&queue->queue_lock);
  return dead;
}

size_t work_queue_task_count(struct work_queue *queue) {
  size_t count = 0;
  // spinlock_acquire(&queue->queue_lock);
  count = queue->task_count;
  // spinlock_release(&queue->queue_lock);
  return count;
}
