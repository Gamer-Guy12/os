#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "lib/list.h"
#include "lib/queue.h"
#include "lib/rbtree.h"
#include "lib/rw_lock.h"
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
  THREAD_READY,
  // Thread needs to be forked
  THREAD_FORKING
};

struct wait_queue {
  struct list_node list;
  rw_lock_t lock;
};

struct wait_queue_node {
  struct list_node node;
  struct thread *thread;
};

struct thread {
  // Don't put anything before the context
  struct context context;
  struct rbnode id_node;
  struct queue_node node;
  // Threads that are waiting for it to end
  struct wait_queue thread_dependencies;
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

// Waiting
void waitqueue_create(struct wait_queue *queue);
void waitqueue_awaken(struct wait_queue *queue, struct wait_queue_node *thread);

// List node to wait queue node
#define WQ_NODE(node)                                                          \
  (((struct wait_queue_node *)((uintptr_t)node -                               \
                               offsetof(struct wait_queue_node, node))))

#endif
