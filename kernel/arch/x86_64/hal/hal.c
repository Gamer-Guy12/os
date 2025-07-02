#include <hal/hal.h>
#include <hal/irq.h>

void init_x86_64_hal(void) { hal_init_irq(); }
