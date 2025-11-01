kernel-mods += build/mods/kernel.o

kernel-src = kernel/console.c kernel/kprintf.c kernel/mem/buddy.c
kernel-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(kernel-src)))

build/mods/kernel.o: $(kernel-obj)
	$(LD) -r $^ -o $@

