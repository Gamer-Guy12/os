kernel-mods += x86_64_lib.o

x86_64_lib-src = arch/x86_64/lib/spinlock.c arch/x86_64/lib/rw_lock.c

build/mods/x86_64_lib.o: $(addprefix build/obj/, $(patsubst %,%.o,$(x86_64_lib-src)))
	$(LD) -r $^ -o $@

