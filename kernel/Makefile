kernel-mods += kernel.o

kernel-src = kernel/console.c kernel/kprintf.c kernel/mem/buddy.c kernel/mem/page.c kernel/mem/gheap.c

build/mods/kernel.o: $(addprefix build/obj/, $(patsubst %,%.o,$(kernel-src)))
	$(LD) -r $^ -o $@

