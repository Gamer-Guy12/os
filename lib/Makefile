kernel-mods += build/mods/lib.o 

lib-src = lib/string.c lib/atomic.c lib/io.c lib/bit.c
lib-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(lib-src)))

build/mods/lib.o: $(lib-obj)
	$(LD) -r $^ -o $@ 
