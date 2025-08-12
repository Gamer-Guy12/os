#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <interrupts.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

typedef struct {
  queue_t idle_queue;
  queue_t normal_queue;
  rbtree_t priority_queue;
  rbtree_t io_queue;
} thread_queue_t;

/// Loads a new thread
/// Returns the old thread
TCB_t *switch_threads(TCB_t *thread);
/// If a thread has state started then instead u want to start it
/// Returns the old thread
TCB_t *start_thread(TCB_t *thread);

/// Create process calls this
void store_process(PCB_t *pcb);
/// Delete process calls this
void remove_process(PCB_t *pcb);
PCB_t *get_proc_list(void);
void clear_processes(void);

void queue_thread(TCB_t *tcb, thread_priority_t priority,
                  thread_queue_t *queue);
TCB_t *pop_thread(thread_queue_t *queue);

void init_threading(void);

/// This is the function that new threads should go to, it will set up the
/// thread and then ret to it
void begin_thread(TCB_t *old);

#endif
