CFLAGS += -mno-sse -mno-avx -mno-mmx -mno-red-zone

kernel-mods += x86_64.o

x86_64-src = arch/x86_64/cores.c \
	arch/x86_64/interrupts.c \
	arch/x86_64/paging.c \
	arch/x86_64/zones.c \
	arch/x86_64/serial.c \
	arch/x86_64/tables.c \
	arch/x86_64/idt.c \
	arch/x86_64/idt.asm \
	arch/x86_64/apic.c \
	arch/x86_64/panic.c \
	arch/x86_64/switch.asm \
	arch/x86_64/threads.c

build/obj/arch/x86_64/idt.asm.o: arch/x86_64/idt.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

build/obj/arch/x86_64/switch.asm.o: arch/x86_64/switch.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

