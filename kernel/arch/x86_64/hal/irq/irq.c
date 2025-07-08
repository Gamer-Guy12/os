#include <cls.h>
#include <hal/irq.h>
#include <libk/err.h>
#include <libk/sys.h>

hal_irq_t irq;

void hal_init_irq(void) {
  if (check_apic()) {
    irq = init_apic();
    return;
  }

  sys_panic(HAL_INIT_ERR | IRQ_HANDLING_ERR);
}

hal_irq_t get_hal_irq(void) {
  return irq;
}
