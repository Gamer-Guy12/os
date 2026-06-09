#ifndef _x86_64_IDT_H_
#define _x86_64_IDT_H_

#include <stdint.h>

#define IDT_INTERRUPT 0xE
// Use this one if you want to let interrupts happen during this handler
#define IDT_TRAP 0xF

typedef struct {
  // Size - 1
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) idt_descriptor_t;

struct int_descriptor {
  uint16_t offset_0;
  uint16_t segment;
  uint16_t ist : 3;
  uint16_t reserved_0 : 5;
  uint16_t gate_type : 4;
  uint16_t reserved_1 : 1;
  uint16_t dpl : 2;
  uint16_t present : 1;
  uint16_t offset_1;
  uint32_t offset_2;
  uint32_t reserved_2;
} __attribute__((packed));

struct int_context {
  uint64_t ds;
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t int_number;
  uint64_t error_code;
  uint64_t rip, cs, rflags;
  uint64_t rsp, ss;
} __attribute__((packed));

void common_handler(struct int_context *context);
void init_idt(void);

#endif
