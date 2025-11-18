#ifndef _x86_64_INTERRUPTS_H_
#define _x86_64_INTERRUPTS_H_

#include <stdint.h>

typedef struct {
  // Size - 1
  uint16_t size;
  uint64_t addr;
} __attribute__((packed)) idtptr_t;

#define IDT_GATE_INT 0xE
#define IDT_GATE_TRAP 0xF

struct idt_descriptor {
  uint16_t offset_1;
  uint16_t segment;
  uint16_t ist : 3;
  uint16_t reserved_1 : 5;
  uint8_t gate_type : 4;
  uint8_t reserved_2 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_3;
} __attribute__((packed));

struct int_context {
  uint64_t int_number;
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t error_code;
  uint64_t rip, cs, rflags;
  uint64_t rsp, ss;
} __attribute__((packed));

void init_interrupts(void);
void common_int_handler(struct int_context *context);

#endif
