#ifndef X86_64_HAL_IRQ_H
#define X86_64_HAL_IRQ_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  void (*eoi)(void);
  void (*map_irq)(uint8_t interrupt_number, uint8_t gate_type);
  void (*mask_irq)(uint32_t irq);
  void (*unmask_irq)(uint32_t irq);
  void (*mask_all_irqs)(void);
} hal_irq_t;

void hal_init_irq(void);
hal_irq_t get_hal_irq(void);

/// List of check functions
bool check_apic(void);
bool check_xapic(void);
bool check_x2apic(void);

/// List of init functions for different types
/// They return all their usages and the hal gives them out
hal_irq_t init_apic(void);
hal_irq_t init_xapic(void);
hal_irq_t init_x2apic(void);

#endif
