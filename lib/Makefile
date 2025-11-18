kernel-mods += lib.o 

lib-src = lib/string.c lib/atomic.c lib/io.c lib/bit.c lib/freelist.c lib/list.c

build/mods/lib.o: $(addprefix build/obj/, $(patsubst %,%.o,$(lib-src)))
	$(LD) -r $^ -o $@ 
