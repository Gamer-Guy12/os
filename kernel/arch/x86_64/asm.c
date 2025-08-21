#include <asm.h>
#include <stddef.h>
#include <stdint.h>

static int64_t uses = 0;

void __cli(void) {
  ASM("cli");
  ATOMIC_INC(uses);
}

void __sti(void) {
  /// This is the value that is currently in uses
  int64_t val = ATOMIC_DEC(uses) - 1;

  if (val <= 0) {
    ASM("sti");
  }

  if (val < 0) {
    CAS(uses, val, 0);
  }
}
