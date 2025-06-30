#include <libk/sys.h>
#include <libk/err.h>
#include <cls.h>
#include <hal/irq.h>

void hal_init_irq(void) {
  cls_t* cls = get_cls();

  if (check_apic()) {
    cls->irq_interface = init_apic();
    return;
  }

  sys_panic(HAL_INIT_ERR | IRQ_HANDLING_ERR);
}

hal_irq_t get_hal_irq(void) {
  cls_t* cls = get_cls(); 

  return cls->irq_interface;
}

