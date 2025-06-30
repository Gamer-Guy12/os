#include <cls.h>
#include <hal/irq.h>

void hal_init_irq(void) {
  
}

hal_irq_t get_hal_irq(void) {
  cls_t* cls = get_cls(); 

  return cls->irq_interface;
}

