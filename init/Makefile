kernel-mods += init.o 

init-src = init/init.c init/mem.c init/fmem.c

build/mods/init.o: $(addprefix build/obj/, $(patsubst %,%.o,$(init-src)))
	$(LD) -r $^ -o $@ -T init/linker.ld

