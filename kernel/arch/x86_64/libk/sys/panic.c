#include <decls.h>
#include <libk/err.h>
#include <libk/kio.h>
#include <libk/sys.h>
#include <stdbool.h>
#include <stdint.h>

void NORETURN sys_panic(uint64_t error_code) {
  kio_printf("The Kernel is in panic. Error Code: %x\n", error_code);
  kio_printf("Isolated Code %x\n", GET_CODE(error_code));

  // Wait infinitely
  while (true) {
    __asm__ volatile("cli; hlt;");
  }
}
