/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <hal/irq.h>
#include <hal/kbd.h>

typedef struct {
  hal_irq_t irq_interface;
  hal_kbd_t kbd_interface;
} cls_t;

void init_cls(void);
cls_t *get_cls(void);

#endif
