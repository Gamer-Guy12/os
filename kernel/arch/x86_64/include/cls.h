/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <decls.h>
#include <gdt.h>
#include <hal/irq.h>
#include <hal/kbd.h>
#include <interrupts.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

typedef struct cls_struct {
  ALIGN(0x10) idt_gate_descriptor_t idt[256];
  ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];
  tss_t *tss;
  TCB_t* tcb_queue;
  TCB_t* tcb_queue_end;
  struct cls_struct *next;
} cls_t;

void init_cls(void);
cls_t *get_cls(void);
cls_t *get_cls_list(void);
size_t get_cls_count(void);

#endif
