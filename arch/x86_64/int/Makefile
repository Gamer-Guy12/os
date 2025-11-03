kernel-mods += build/mods/x86_64_int.o

x86_64_int-src = arch/x86_64/int/interrupts.c
x86_64_int-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(x86_64_int-src)))

build/mods/x86_64_int.o: $(x86_64_int-obj)
	$(LD) -r $^ -o $@

