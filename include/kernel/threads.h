#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "lib/queue.h"
#include "util.h"
#include <stdint.h>

#define STACK_ORDER 2

enum thread_state {
  // Currently running
  THREAD_RUNNING,
  // Currently waiting
  THREAD_WAITING,
  // Thread dead
  THREAD_TERMINATED,
  // It is in the queue
  THREAD_READY
};

struct thread {
  // Don't put anything before the context
  struct context context;
  struct queue_node node;
  uint64_t tid;
  // Used for freeing the stack
  void *stack;
  void (*entry)(void);
  pt_t page_tables;
  enum thread_state state;
};

struct thread_queue {
  struct queue queue;
};

// Arch dependent switch
// The parameter registers at the end should contain the old and new threads
// Ex: (on x86_64) rdi: old thread, rsi: new thread
void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __switch_pages(pt_t tables);
int __pages_null(pt_t tables);
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
pt_t __null_pages(void);

// Switch
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void switch_tail(struct thread *old_thread, struct thread *new_thread);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);

// Utils
struct thread *get_cur_thread(void);

// Lifecycle
// Puts the thread into thread queues
struct thread *create_thread(void (*entry)(void));
void destroy_thread(struct thread *thread);

// Init
void init_general_threading(void);
void init_threading(void *stack);

// Queueing
void init_thread_queue(struct thread_queue *queue);
void init_local_thread_queue(void);
void init_global_thread_queue(void);
struct thread *pop_queue_thread(struct thread_queue *queue);
void queue_thread(struct thread_queue *queue, struct thread *thread);
void schedule_thread(struct thread *thread);
void requeue_thread(struct thread *thread);
struct thread *pop_thread(void);
// Gets a thread from the global queue
void get_thread(void);

// Scheduling
// Switch to new thread
void schedule(void);
// Kills current thread
NORETURN void terminate(void);

#endif
