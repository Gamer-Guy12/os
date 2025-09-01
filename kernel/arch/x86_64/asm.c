#include <asm.h>
#include <cls.h>
#include <stddef.h>
#include <stdint.h>

void __cli(void) {
  cls_t *cls = get_cls();

  if (cls == NULL) {
    ASM("cli");
    return;
  }

  ASM("cli");
  ATOMIC_INC(cls->interrupt_flag_uses);
}

void __sti(void) {
  cls_t *cls = get_cls();
  if (cls == NULL) {
    ASM("sti");
    return;
  }

  int64_t val = 0;

  do {
    val = ATOMIC_LOAD(&cls->interrupt_flag_uses);
    if (val == 0) {
      return;
    }
  } while (!CAS(cls->interrupt_flag_uses, val, val - 1));

  ASM("sti");
}
