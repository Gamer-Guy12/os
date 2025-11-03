kernel-mods += build/mods/x86_64_lib.o

x86_64_lib-src = arch/x86_64/lib/spinlock.c
x86_64_lib-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(x86_64_lib-src)))

build/mods/x86_64_lib.o: $(x86_64_lib-obj)
	$(LD) -r $^ -o $@

