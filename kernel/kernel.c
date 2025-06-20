#include <decls.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

void NORETURN kernel_main(void) {
  // kio_clear();
  kio_printf("We are in Kernel Main!\n");

  // uint64_t *num1 = kmalloc(sizeof(uint64_t), 0);
  // *num1 = 29;
  //
  // uint64_t *num2 = kmalloc(sizeof(uint64_t), 0);
  // *num2 = 65;
  //
  // uint64_t *nums = kmalloc(sizeof(uint64_t) * 2, 0);
  // nums[0] = 489;
  // nums[1] = 48;
  //
  // kio_printf("Malloced number is %u %u %u %u\n", *num1, *num2, nums[0],
  //            nums[1]);
  //
  // kio_printf("Malloc %u\n", *num2);
  //
  // kfree(num2);
  //
  // uint64_t *num3 = kmalloc(sizeof(uint64_t), 0);
  // *num3 = 48;
  // kio_printf("Malloc %u\n", *num1);
  // kio_printf("Malloc %u\n", *num3);

  while (1) {
  }
}
