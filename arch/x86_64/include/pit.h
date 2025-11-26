#ifndef _x86_64_PIT_H_
#define _x86_64_PIT_H_

#include "asm.h"
#include <stdint.h>

#define PIT_16_BINARY 0
#define PIT_4_BCD 1

#define PIT_MODE_0 (0 << 1)
#define PIT_MODE_1 (1 << 1)
#define PIT_MODE_2 (2 << 1)
#define PIT_MODE_3 (3 << 1)
#define PIT_MODE_4 (4 << 1)
#define PIT_MODE_5 (5 << 1)
// Same as PIT_MODE_2
#define PIT_MODE_2_2 (6 << 1)
// Same as PIT_MODE_3
#define PIT_MODE_3_2 (7 << 1)

#define PIT_LATCH_COUNT_VAL (0 << 4)
#define PIT_LOBYTE (1 << 4)
#define PIT_HIBYTE (2 << 4)
#define PIT_LOHIBYTE (3 << 4)

#define PIT_CHANNEL_0 (0 << 6)
#define PIT_CHANNEL_1 (1 << 6)
#define PIT_CHANNEL_2 (2 << 6)
#define PIT_READ_BACK (3 << 6)

#define PIT_RB_NO_LATCH_COUNT (1 << 5)
#define PIT_RB_NO_LATCH_STATUS (1 << 4)
#define PIT_RB_CHANNEL_2 (1 << 3)
#define PIT_RB_CHANNEL_1 (1 << 2)
#define PIT_RB_CHANNEL_0 (1 << 1)

#define PIT_DATA_0 0x40
#define PIT_DATA_1 0x41
#define PIT_DATA_2 0x42
#define PIT_COMMAND 0x43

#define WRITE_PIT_DATA_0(data) OUTB(0x40, data)
#define WRITE_PIT_DATA_1(data) OUTB(0x41, data)
#define WRITE_PIT_DATA_2(data) OUTB(0x42, data)
#define WRITE_PIT_COMMAND(command) OUTB(0x43, command)

#define PIT_FREQUENCY 1193182

void init_pit(void);
void pit_wait_ticks(uint16_t ticks);
uint16_t read_ticks(void);
uint16_t ms_to_ticks(uint8_t ms);

#endif
