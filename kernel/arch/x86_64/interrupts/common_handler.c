#include <interrupts.h>
#include <libk/kio.h>

void common_interrupt_handler(idt_registers_t *registers) {
  kio_printf("Interrupt %x\n", registers->interrupt_number);
}
