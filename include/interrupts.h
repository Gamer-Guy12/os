#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

void _disable_interrupts(void);
void _enable_interrupts(void);

void disable_interrupts(void);
void enable_interrupts(void);

// The void * points to arch specific
// on x86_64 it is the interrupt context
void register_interrupt(void (*handler)(void *), int interrupt);

#endif

