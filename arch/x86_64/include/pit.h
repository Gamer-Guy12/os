#ifndef _x86_64_PIT_H_
#define _x86_64_PIT_H_

#include <stdint.h>
#include <stdbool.h>

#define PIT_FREQ 1193182

#define PIT_CHANNEL_0 0x40
#define PIT_CHANNEL_1 0x41
#define PIT_CHANNEL_2 0x42
#define PIT_COMMAND 0x43

#define PIT_BINARY (0 << 0)
#define PIT_BCD (1 << 0)
// Modes
// Interrupt on terminal count
#define PIT_MODE0 (0 << 1)
// Hardware retrigerrable oneshot
#define PIT_MODE1 (1 << 1)
// Rate generator
#define PIT_MODE2 (2 << 1)
// Square wave generator
#define PIT_MODE3 (3 << 1)
// Software triggered strobe
#define PIT_MODE4 (4 << 1)
// Hardware triggered strobe
#define PIT_MODE5 (5 << 1)
// Rate generator
#define PIT_MODE6 (6 << 1)
// Square generator
#define PIT_MODE7 (7 << 1)
// Access mode
#define PIT_LATCH (0 << 4)
#define PIT_LOBYTE (1 << 4)
#define PIT_HIBYTE (2 << 4)
#define PIT_LOHIBYTE (3 << 4)
// Channel
#define PIT_SELECT0 (0 << 6)
#define PIT_SELECT1 (1 << 6)
#define PIT_SELECT2 (2 << 6)
#define PIT_READBACK (3 << 6)

// Starts a countdown (safe for multicore)
void pit_prep(uint16_t ms);
// Ends the countdown by waiting
void pit_count(void);

#endif

