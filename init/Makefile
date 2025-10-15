kernel-objs += build/obj/init.o 

init-src = init/init.c
init-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(init-src)))

build/obj/init.o: $(init-obj)
	$(LD) -r $^ -o build/obj/init.o -T init/linker.ld

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

