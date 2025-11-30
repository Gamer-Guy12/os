CFLAGS += -mno-sse -mno-avx -mno-mmx -mno-red-zone

kernel-mods += x86_64.o

x86_64-src = arch/x86_64/cores.c \
	arch/x86_64/init.c \
	arch/x86_64/interrupts.c \
	arch/x86_64/gdt.c \
	arch/x86_64/handlers.asm \
	arch/x86_64/apic.c \
	arch/x86_64/tsc.c \
	arch/x86_64/pit.c \
	arch/x86_64/hpet.c

build/obj/arch/x86_64/handlers.asm.o: arch/x86_64/handlers.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

