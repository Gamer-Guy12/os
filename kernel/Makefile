kernel-mods += 

kernel-src = kernel/console.c
kernel-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(kernel-src)))

build/mods/kernel.o: $(kernel-obj)
	$(LD) -r $^ -o $@

