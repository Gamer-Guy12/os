#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <interrupts.h>
#include <libk/list.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

typedef struct {
  queue_t idle_queue;
  queue_t normal_queue;
  rbtree_t priority_queue;
  rbtree_t io_queue;
} thread_queue_t;

typedef struct semaphore_struct {
  thread_queue_t queue;
  int64_t current_count;
} semaphore_t;

static inline void thread_queue_create(thread_queue_t *queue) {
  queue_create(&queue->idle_queue);
  queue_create(&queue->normal_queue);
  rb_create(&queue->priority_queue);
  rb_create(&queue->io_queue);
}

/// Loads a new thread
/// Returns the old thread
TCB_t *WUNUSED switch_threads(TCB_t *thread);
/// If a thread has state started then instead u want to start it
/// Returns the old thread
TCB_t *WUNUSED start_thread(TCB_t *thread);

/// Create process calls this
void store_process(PCB_t *pcb);
/// Delete process calls this
void remove_process(PCB_t *pcb);
PCB_t *WUNUSED get_proc_list(void);
void clear_processes(void);

void queue_thread(TCB_t *tcb, thread_priority_t priority,
                  thread_queue_t *queue);
TCB_t *WUNUSED pop_thread(thread_queue_t *queue);

void init_threading(void);
void init_sleep(void);

/// This is the function that new threads should go to, it will set up the
/// thread and then ret to it
void begin_thread(TCB_t *old);

/// Returns the event handle
size_t schedule_event(uint64_t ms, void *data, void (*handler)(void *));
void cancel_event(size_t handle);
void init_events(void);

void run_preemption(void);
void enable_preemption(void);
void disable_preemption(void);

#endif
