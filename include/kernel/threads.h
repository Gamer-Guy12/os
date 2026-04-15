#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "lib/list.h"
#include "lib/queue.h"
#include "lib/rbtree.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stdint.h>
#include <stdbool.h>

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

struct wait_queue {
  struct list_node list;
  spinlock_t lock;
};

struct thread {
  // Don't put anything before the context
  struct context context;
  struct rbnode id_node;
  struct queue_node node;
  struct list_node wait_queue_node;
  uint64_t tid;
  union {
    size_t event_filter;
    size_t event_return;
  };
  struct wait_queue *wait_queue;
  // Used for freeing the stack
  void *stack;
  void (*entry)(void);
  pt_t page_tables;
  enum thread_state state;
  int exit_code;
};

struct thread_queue {
  struct queue queue;
};

struct event {
  struct wait_queue queue;
  struct list_node threads;
  spinlock_t lock;
  bool uses_filter;
};

struct event_node {
  struct list_node node;
  uint64_t tid;
  size_t filter;
};

// Arch dependent switch
// The parameter registers at the end should contain the old and new threads
// Ex: (on x86_64) rdi: old thread, rsi: new thread
void __switch_context(struct context *old_ctx, struct context *new_ctx);
void __switch_pages(pt_t tables);
int __pages_null(pt_t tables);
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
void __copy_context(struct thread *old_ctx, struct thread *new_ctx);
pt_t __null_pages(void);

// Switch
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void switch_tail(struct thread *old_thread, struct thread *new_thread);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);

// Utils
struct thread *get_cur_thread(void);
// Gets a thread from its id
struct thread *thread_id(uint64_t id);

// Lifecycle
// Puts the thread into thread queues
uint64_t create_thread(void (*entry)(void));
void destroy_thread(struct thread *thread);

// Init
void init_general_threading(void);
// Used to get the cores into the threading system
void init_threading(void *stack);

// Queueing
void init_thread_queue(struct thread_queue *queue);
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
NORETURN void terminate(int code);
int wait_thread(uint64_t tid);

// Waiting
void waitqueue_create(struct wait_queue *queue);
bool waitqueue_destroy(struct wait_queue *queue, bool force);
void waitqueue_awaken(struct wait_queue *queue, uint64_t tid);
// Inserts current thread into waitqueue
void waitqueue_wait(struct wait_queue *queue);

// Sleep
void init_sleep(void);
void sleep(uint32_t ms);

// Events
void event_create(struct event *event, bool uses_filter);
void event_destroy(struct event *event);
void event_trigger(struct event *event, size_t filter, size_t value);
// Returns value
size_t event_wait(struct event *event, size_t filter);

#endif
