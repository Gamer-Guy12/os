/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <hal/irq.h>
#include <hal/kbd.h>
#include <decls.h>
#include <gdt.h>
#include <interrupts.h>

typedef struct {
  hal_irq_t irq_interface;
  hal_kbd_t kbd_interface;
  ALIGN(0x8) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];
  ALIGN(0x10) idt_gate_descriptor_t idt[256];
} cls_t;

void init_cls(void);
cls_t *get_cls(void);

#endif
