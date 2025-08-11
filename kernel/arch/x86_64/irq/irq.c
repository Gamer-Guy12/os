#include <cls.h>
#include <irq.h>
#include <libk/err.h>
#include <libk/sys.h>

irq_t irq;

void init_irq(void) {
  if (check_apic()) {
    irq = init_apic();
    return;
  }

  sys_panic(HAL_INIT_ERR | IRQ_HANDLING_ERR);
}

irq_t get_irq(void) {
  return irq;
}
