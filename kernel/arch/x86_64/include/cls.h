/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <threading/pcb.h>
#include <threading/tcb.h>
#include <hal/irq.h>
#include <hal/kbd.h>
#include <decls.h>
#include <gdt.h>
#include <interrupts.h>

typedef struct {
  ALIGN(0x10) idt_gate_descriptor_t idt[256];
  ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];
  PCB_t* processes;
  PCB_t* processes_end;
  TCB_t* thread_queue;
  TCB_t* thread_queue_end;
} cls_t;

void init_cls(void);
cls_t *get_cls(void);

#endif
