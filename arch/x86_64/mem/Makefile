kernel-mods += build/mods/x86_64_mem.o

x86_64_mem-src = arch/x86_64/mem/init_paging.c arch/x86_64/mem/zones.c
x86_64_mem-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(x86_64_mem-src)))

build/mods/x86_64_mem.o: $(x86_64_mem-obj)
	$(LD) -r $^ -o $@

