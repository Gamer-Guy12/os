kernel-mods += x86_64_threads.o

x86_64_threads-src = arch/x86_64/threads/switch.asm \
	arch/x86_64/threads/context.c

build/obj/arch/x86_64/threads/switch.asm.o: arch/x86_64/threads/switch.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

