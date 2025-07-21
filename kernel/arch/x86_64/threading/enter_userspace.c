#include <threading/tcb.h>
#include <asm.h>
#include <cls.h>
#include <gdt.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/userspace.h>

extern void userspace_jump(tss_t *tss);

#define FS_MSR 0xC0000100

void jump_to_userspace(void) {
  // get the addr to save rsp at
  tss_t *tss = get_cls()->tss;

  userspace_jump(tss);
}

