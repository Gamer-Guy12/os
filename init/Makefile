kernel-mods += build/mods/init.o 

init-src = init/init.c init/mem.c
init-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(init-src)))

build/mods/init.o: $(init-obj)
	$(LD) -r $^ -o $@ -T init/linker.ld

