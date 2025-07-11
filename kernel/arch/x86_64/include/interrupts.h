#ifndef X86_64_INTERRUPTS
#define X86_64_INTERRUPTS

#include <decls.h>
#include <stdbool.h>
#include <stdint.h>

#define IDT_INTERRUPT_GATE 0xE
#define IDT_TRAP_GATE 0xF

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
} PACKED idt_descriptor_t;

/// Everything is pushed to the stack in reverse order
typedef struct {
  uint64_t ds;
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rsi, rdi, rbp, rdx, rcx, rbx, rax;
  uint64_t interrupt_number;
  uint64_t error_code;
  uint64_t rip, cs, rflags;
  /// If the interrupt was from user space we would also have
  ///
  ///  Don't use unless this is garuanteed form user space
  /// 
  /// First check if it is from userspace
  uint64_t rsp, ss;
} PACKED idt_registers_t;

typedef void (*interrupt_handler_t)(idt_registers_t *registers);

void set_descriptor_offset(idt_gate_descriptor_t *descriptor, uint64_t offset);

/// Common Interrupt Handler
///
void common_interrupt_handler(idt_registers_t *registers);

void set_idt_gate(uint8_t gate_number, void (*handler)(void),
                  uint16_t seg_descriptor, uint8_t gate_type, bool present,
                  uint8_t dpl, uint8_t ist);
void enable_idt_gate(uint8_t gate_number);

void register_handlers(void);

void init_interrupts(void);

void load_idt(void);

// If handler already exists it just sets it to a new one, so make sure u don't
// already have things
void register_interrupt_handler(interrupt_handler_t handler, uint8_t index);

#endif
