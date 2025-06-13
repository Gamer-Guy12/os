#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

size_t brk = 0;
#define PAGE_SIZE 0x1000

#define ROUND_UP(num, to) ((num) + ((to) - ((num) % (to))))
#define ROUND_DOWN(num, to) ((num) - ((num) % (to)))

bool increment(size_t amount) {}

int main(void) {}
