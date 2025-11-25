#ifndef _INIT_H_
#define _INIT_H_

void arch_init(void);
// Only init on one core
// On that core, arch init is not called
void arch_init_single(void);

#endif

