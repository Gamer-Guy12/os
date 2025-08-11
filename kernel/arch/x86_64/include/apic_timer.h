/// The apic timer is reserved for preemption

#ifndef X86_64_APIC_TIMER_H
#define X86_64_APIC_TIMER_H

void init_apic_timer(void);
void start_preemption(void);

void enable_preemption(void);
void disable_preemption(void);

#endif

