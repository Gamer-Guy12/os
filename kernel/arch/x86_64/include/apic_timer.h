/// The apic timer is reserved for preemption

#ifndef X86_64_APIC_TIMER_H
#define X86_64_APIC_TIMER_H

#include <stddef.h>

void init_apic_timer(void);
void apic_interrupt_at(size_t tsc_deadline, void (*callback)(void));
size_t ms_to_deadline(size_t ms);
void pause_apic_timer(void);

#endif

