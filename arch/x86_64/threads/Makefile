kernel-mods += x86_64_threads.o

x86_64_threads-src = arch/x86_64/threads/switch.c arch/x86_64/threads/switch.asm

build/obj/arch/x86_64/threads/switch.asm.o: arch/x86_64/threads/switch.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

