#ifndef X86_64_TCB_H
#define X86_64_TCB_H

#include <libk/queue.h>
#include <gdt.h>
#include <interrupts.h>
#include <stddef.h>
#include <threading/pcb.h>

typedef enum {
  THREAD_RUNNING = 1,
  THREAD_WAITING = 2,
  THREAD_TERMINATED = 3,
  THREAD_STARTING = 4
} TCB_state_t;

typedef struct {
  size_t rax, rbx, rcx, rdx, rbp, rdi, rsi;
  size_t r8, r9, r10, r11, r12, r13, r14, r15;
  size_t cs, ds, es, ss;
  size_t rflags;
} registers_t;

typedef struct TCB_struct {
  size_t tid;
  registers_t *registers;
  size_t stack_num;
  size_t rsp;
  size_t rsp0;
  size_t rip;
  size_t rip0;
  TCB_state_t state;
  PCB_t *pcb;
  struct TCB_struct *next;
  struct TCB_struct *prev;
  queue_node_t node;
} TCB_t;

#endif
