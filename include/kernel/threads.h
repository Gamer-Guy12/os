#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "lib/atomic.h"
#include "lib/list.h"
#include "lib/queue.h"
#include "lib/rbtree.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stdint.h>

#define STACK_ORDER 2

#ifdef _x86_64_
typedef uint64_t tid_t;
#else
typedef uint32_t tid_t;
#endif

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

enum thread_priority {
  // Used for work queues that are handling interrupts and stuff
  TP_INTERRUPT,
  // Used for important stuff (like handling timer interrupts)
  TP_HIGH,
  // Used for normal threads
  TP_NORMAL,
  // Used for the idle threads
  TP_IDLE,
  TP_COUNT,
  TP_NO_QUEUE
};

struct thread {
  // Don't put anything before the context
  struct context context;
  struct rbnode id_node;
  struct list_node wait_node;
  struct queue_node queue_node;
  tid_t tid;
  // Used for freeing the stack
  void *stack;
  void (*entry)(void);
  struct thread *prev;
  union {
    struct wait_queue *wait_queue;
    // Only used on entry
    void *data;
  };
  size_t event_filter;
  pt_t page_tables;
  enum thread_state state;
  enum thread_priority priority;
  int exit_code;
};

struct thread_queue {
  struct queue queues[TP_COUNT];
  spinlock_t queue_lock;
};

struct wait_queue {
  struct list_node waiting_threads;
  // 2 bits
  // bit 0: 0 means continue, 1 means stop
  // bit 1: 1 means accept, 0 means don't
  int (*check_thread)(struct thread *thread, void *data);
  atomic_t wait_count;
  spinlock_t wait_lock;
};

struct event {
  struct wait_queue queue;
  bool uses_filter;
};

#define DEF_TASKS_THREAD 30

struct work_task {
  struct queue_node node;
  void (*task)(void *data);
  void *data;
};

struct work_queue {
  enum thread_priority priority;
  struct wait_queue workers;
  struct queue tasks;
  size_t task_count;
  size_t worker_threads;
  size_t tasks_per_thread;
  // 0 alive, 1 dying, 2 dead
  size_t state;
  spinlock_t queue_lock;
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

// Utils
struct thread *get_cur_thread(void);
struct thread *thread_id(tid_t tid);

// Switch
void switch_threads(struct thread *old_thread, struct thread *new_thread);
void switch_tail(void);
void thread_trampoline(struct thread *old_thread, struct thread *new_thread);

// Init
void init_threading(void *stack);
void init_thread_queues(void);
void init_thread_queue(struct thread_queue *queue);

// Creation
struct thread *create_thread(void (*entry)(void),
                             enum thread_priority priority);
void destroy_thread(struct thread *thread);

// Queueing
struct thread *pop_queue_thread(struct thread_queue *queue);
void enqueue_thread(struct thread_queue *queue, struct thread *thread);
struct thread *pop_thread(void);
void schedule_thread(struct thread *thread);
void requeue_thread(struct thread *thread);

// Lifecycle
// Guarenteed to switch (unless your an idle thread)
void init_thread_lifecycle(void);
void schedule(void);
void terminate(int code);
int wait_thread(tid_t thread);
void __trigger_death_event(tid_t thread, int code);

// Wait
// Wait current thread
void waitqueue_create(struct wait_queue *queue,
                      int (*check_thread)(struct thread *, void *));
void waitqueue_wait(struct wait_queue *queue);
void waitqueue_awaken(struct wait_queue *queue, void *data);
void waitqueue_awaken_all(struct wait_queue *queue);
// force means whether to return if there are still threads or awaken all
// threads
bool waitqueue_destroy(struct wait_queue *queue, bool force);
void __insert_wait_thread(struct thread *thread);

// Event
void event_create(struct event *event, bool uses_filter);
void event_wait(struct event *event, size_t filter);
void event_trigger(struct event *event, size_t filter);
void event_destroy(struct event *event, bool force);

// Work Queues
// tasks_per_thread, when set to 0 defaults to DEF_TASKS_THREAD
void work_queue_create(struct work_queue *queue, enum thread_priority priority,
                       size_t tasks_per_thread);
void work_queue_add(struct work_queue *queue, void (*task)(void *), void *data);
void work_queue_destroy(struct work_queue *queue);
bool work_queue_dead(struct work_queue *queue);
size_t work_queue_task_count(struct work_queue *queue);

#endif
