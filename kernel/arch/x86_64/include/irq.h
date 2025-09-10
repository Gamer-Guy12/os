#ifndef X86_64_HAL_IRQ_H
#define X86_64_HAL_IRQ_H

#include <decls.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  void (*eoi)(void);
  /// This maps the isa irq so if there is a difference this will autofix
  void (*map_irq)(uint8_t interrupt_number, uint8_t irq_number);
  void (*mask_irq)(uint32_t irq);
  void (*unmask_irq)(uint32_t irq);
  void (*mask_all_irqs)(void);
  void (*set_trigger_mode)(bool edge, bool active_low, uint32_t irq);
  /// This is mostly for the apic
  /// it gives an irq value that can be used to get the irq u want because they
  /// are usually mapped through isa thingy stuff ig
  uint8_t (*get_pass_irq)(uint8_t wanted_irq);
} irq_t;

void init_irq(void);
irq_t WUNUSED get_irq(void);

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
