#ifndef _KERNEL_THREADS_H_
#define _KERNEL_THREADS_H_

#include "arch/threads.h"
#include "lib/list.h"
#include "lib/queue.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

#ifdef _x86_64_
typedef uint64_t tid_t;
#else
typedef uint32_t tid_t;
#endif

// How big the stack is
#define STACK_ORDER 1

enum thread_priority {
  // Will not run unless there is nothing else left (used for idle threads)
  THREAD_IDLE,
  // What a thread normally needs
  THREAD_NORMAL,
  // The highest priority
  THREAD_HIGH,
  PRIORITY_COUNT,
  // Don't schedule this thread (used during creation)
  THREAD_NO_SCHED
};

enum thread_state {
  THREAD_RUNNING,
  THREAD_READY,
  THREAD_TERMINATED,
  THREAD_WAITING
};

struct thread {
  struct context context;
  struct queue_node node;
  struct list_node wait_node;
  struct waitqueue *waitqueue;
  tid_t tid;
  struct thread *prev;
  void (*entry)(void *);
  // Parameter passed in at entry
  void *param;
  // Used for freeing
  void *stack;
  // Page tables
  pt_t pages;
  int state;
  int priority;
  int exit_code;
};

struct thread_queue {
  struct queue queues[PRIORITY_COUNT];
};

struct waitqueue {
  struct list_node threads;
  // the void * is an arbitrary parameter
  //
  // Return value
  // bit 0 says whether the check is true or not
  // bit 1 says whether to continue or stop checking
  int (*check_thread)(struct thread *, void *);
  size_t thread_count;
  spinlock_t lock;
};

void init_threading(void *stack);

// Arch specific
void __switch_context(struct context *old, struct context *new);
void __switch_pages(pt_t pages);
// Returns a null page table
pt_t __pages_null(void);
// Gets the current page tables
pt_t __cur_pages(void);
void __create_context(struct thread *thread);
void __switch_stacks(void (*entry)(void *));

// Switching
void switch_threads(struct thread *old, struct thread *new);
// Called at the end of a thread switch
void switch_tail(void);

// Lifecycle
struct thread *create_thread(void (*entry)(void *), void *param, int priority);
// Can not be in any queues or running
void destroy_thread(struct thread *thread);
void thread_trampoline(void);

// Queue
// Sets the thread->priority
void init_queues(void);
void schedule_thread(struct thread *thread, int priority);
// Get the next thread
struct thread *pop_thread(void);
void queue_thread(struct thread *thread, struct thread_queue *queue,
                  int priority);
struct thread *dequeue_thread(struct thread_queue *queue);

// Waiting
void waitqueue_create(struct waitqueue *queue,
                      int (*check_thread)(struct thread *, void *));
void waitqueue_wait(struct waitqueue *queue);
void waitqueue_awaken(struct waitqueue *queue, void *param);
// Force will awaken all threads but if it is false then the destruction will
// fail
//
// 0 Failure
// 1 Success
int waitqueue_destroy(struct waitqueue *queue, bool force);
void __do_wait(struct thread *thread);

// Scheduling
void terminate(int code);
void schedule(void);

// Util
struct thread *get_cur_thread(void);

// Preemption
// Counted in milliseconds
#define THREAD_QUANTUM 4
// Implemented on each architecture
// Enables preemption and also resets it (called durings scheduling) to make
// sure that threads get total time slice
void do_preemption(void);
// Must be called by a timer (which is different on each arch, it is the apic
// timer on x86_64)
void preempt(void);

// Set current thread priority
#define THREAD_PRIORITY(n)                                                     \
  do {                                                                         \
    SASSERT((n) < PRIORITY_COUNT, "Invalid thread priority");                  \
    get_cur_thread()->priority = n;                                            \
  } while (0)

#endif
