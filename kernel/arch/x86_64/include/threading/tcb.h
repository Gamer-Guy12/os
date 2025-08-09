#ifndef X86_64_TCB_H
#define X86_64_TCB_H

#include <libk/rbtree.h>
#include <gdt.h>
#include <interrupts.h>
#include <libk/queue.h>
#include <stddef.h>
#include <threading/pcb.h>

typedef enum {
  TP_IDLE,
  TP_NORMAL,
  TP_HIGH,
  TP_IO
} thread_priority_t;

typedef enum {
  THREAD_RUNNING = 1,
  THREAD_WAITING = 2,
  THREAD_TERMINATED = 3,
  THREAD_STARTING = 4
} TCB_state_t;

typedef struct {
  size_t rflags;
  size_t rbx, rbp, r12, r13, r14, r15;
  size_t rip0;
} registers_t;

// The quantum count is stored in the rb node
typedef struct TCB_struct {
  size_t tid;
  registers_t *registers;
  size_t stack_num;
  size_t rsp;
  size_t rsp0;
  size_t rip;
  size_t entry_point;
  TCB_state_t state;
  PCB_t *pcb;
  struct TCB_struct *next;
  struct TCB_struct *prev;
  queue_node_t queue_node;
  void* xsave_page;
  rbnode_t rb_node;
  thread_priority_t priority;
} TCB_t;

#endif
