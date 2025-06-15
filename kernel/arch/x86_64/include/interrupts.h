#ifndef X86_64_INTERRUPTS
#define X86_64_INTERRUPTS

#include <decls.h>
#include <stdint.h>

#define IDT_INTERRUPT_GATE 0b1110
#define IDT_TRAP_GATE 0b1111

typedef struct {
  uint16_t offset_1;
  uint16_t selector;
  uint8_t ist : 3;
  uint8_t reserved_1 : 5;
  uint8_t gate_type : 4;
  uint8_t reserved_2 : 1;
  uint8_t dpl : 2;
  uint8_t present : 1;
  uint16_t offset_2;
  uint32_t offset_3;
  uint32_t reserved_3;
} PACKED idt_gate_descriptor_t;

typedef struct {
  /// Size - 1
  uint16_t size;
  uint64_t offset;
} PACKED idt_descriptor;

/// Everything is pushed to the stack in reverse order
typedef struct {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t interrupt_number;
  uint64_t error_code;
  uint64_t rip, cs, rflags;
  // If the interrupt was from user space we would also add
  // uint64_t rsp, ss;
} PACKED idt_registers_t;

void set_descriptor_offset(idt_gate_descriptor_t *descriptor, uint64_t offset);

/// Common Interrupt Handler
///
void common_interrupt_handler(idt_registers_t *registers);

#endif
