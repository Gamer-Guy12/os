/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <decls.h>
#include <gdt.h>
#include <hal/irq.h>
#include <hal/kbd.h>
#include <interrupts.h>
#include <libk/list.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

typedef struct {
  ALIGN(0x10) idt_gate_descriptor_t idt[256];
  ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];
  tss_t *tss;
  interrupt_handler_t *handlers;
  void* true_addr;
  // queue_t idle_queue;
  // rbtree_t normal_queue;
  // rbtree_t priority_queue;
  // queue_t io_queue;
  // queue_t dead_queue;
  // queue_t load_queue;
  list_node_t node;
} cls_t;

void init_cls(void);
cls_t *get_cls(void);
list_t *get_cls_list(void);
cls_t *get_cls_at(size_t index);
size_t get_core_count(void);

#endif
