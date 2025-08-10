#ifndef X86_64_HAL_IRQ_H
#define X86_64_HAL_IRQ_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  void (*eoi)(void);
  /// This maps the isa irq so if there is a difference this will autofix
  void (*map_irq)(uint8_t interrupt_number, uint8_t irq_number);
  void (*mask_irq)(uint32_t irq);
  void (*unmask_irq)(uint32_t irq);
  void (*mask_all_irqs)(void);
  void (*set_edge_triggered)(bool edge, uint32_t irq);
} irq_t;

void init_irq(void);
irq_t get_irq(void);

/// List of check functions
bool check_apic(void);
/// TODO
bool check_xapic(void);
/// TODO
bool check_x2apic(void);

/// List of init functions for different types
/// They return all their usages and the hal gives them out
irq_t init_apic(void);
/// TODO
irq_t init_xapic(void);
/// TODO
irq_t init_x2apic(void);

#endif
