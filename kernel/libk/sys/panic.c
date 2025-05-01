#include <libk/kio.h>
#include <libk/sys.h>
#include <stdbool.h>
#include <stdint.h>

void sys_panic(uint64_t error_code) {
  kio_printf("The Kernel is in panic. Error Code: %u", error_code);

  // Wait infinitely
  while (true)
    ;
}
