CFLAGS += -mno-sse -mno-avx -mno-mmx -mno-red-zone

kernel-mods += build/mods/x86_64.o

x86_64-src = arch/x86_64/cores.c
x86_64-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(x86_64-src)))

build/mods/x86_64.o: $(x86_64-obj)
	$(LD) -r $^ -o $@

