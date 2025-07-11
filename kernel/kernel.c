#include <mem/memory.h>
#include <decls.h>
#include <libk/kio.h>
#include <stddef.h>
#include <stdint.h>

void test_gmalloc(void) {
  uint64_t *ptr1 = gmalloc(8);
  uint64_t *ptr2 = gmalloc(8);

  *ptr1 = 39;
  *ptr2 = 49;

  kio_printf("Ptr1: %u, %x, Ptr2: %u, %x\n", *ptr1, (size_t)ptr1, *ptr2,
             (size_t)ptr2);

  gfree(ptr1);

  uint64_t *ptr3 = gmalloc(8);

  *ptr3 = 290;

  kio_printf("Ptr1: %u, %x, Ptr3: %u, %x\n", *ptr1, (size_t)ptr1, *ptr3,
             (size_t)ptr3);

  gfree(ptr2);
  gfree(ptr3);
}

void NORETURN kernel_main(void) {
  // kio_clear();

  test_gmalloc();

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
