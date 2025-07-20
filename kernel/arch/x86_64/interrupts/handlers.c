#include <cls.h>
#include <interrupts.h>
#include <libk/err.h>
#include <libk/kio.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

void create_handlers(void) {
  cls_t *cls = get_cls();
  cls->handlers = gmalloc(sizeof(interrupt_handler_t) * 256);
}

void common_interrupt_handler(idt_registers_t *registers) {
  interrupt_handler_t *handlers = get_cls()->handlers;

  bool is_exception = registers->interrupt_number < 32;

  if (is_exception && handlers[registers->interrupt_number] == NULL) {
    kio_printf("Exception: number %x, error_code %x, registers:\n",
               registers->interrupt_number, registers->error_code);

    kio_printf("RAX %x, RBX %x, RCX %x, RDX %x\n", registers->rax,
               registers->rbx, registers->rcx, registers->rdx);
    kio_printf("RBP %x, RSI %x, RDI %x\n", registers->rbp, registers->rsi,
               registers->rdi);
    kio_printf("R8 %x, R9 %x, R10 %x, R11 %x\n", registers->r8, registers->r9,
               registers->r10, registers->r11);
    kio_printf("R12 %x, R13 %x, R14 %x, R15 %x\n", registers->r12,
               registers->r13, registers->r14, registers->r15);

    kio_printf("Other info:\n");

    kio_printf("CS %x, DS %x\n", registers->cs, registers->ds);
    kio_printf("RIP %x, RFLAGS %x\n", registers->rip, registers->rflags);
  }

  if (handlers[registers->interrupt_number] == NULL) {
    kio_printf("No handler for interrupt %x!\n", registers->interrupt_number);
    sys_panic(NO_INTERRUPT_HANDLER_ERR);
  }

  handlers[registers->interrupt_number](registers);
}

void register_interrupt_handler(interrupt_handler_t handler, uint8_t index) {
  interrupt_handler_t *handlers = get_cls()->handlers;
  handlers[index] = handler;
}
