kernel-mods += build/mods/x86_64_general.o 

x86_64_general-src = arch/x86_64/general/cores.c 
x86_64_general-obj = $(addprefix build/obj/, $(patsubst %.c,%.o,$(x86_64_general-src)))

build/mods/x86_64_general.o: $(x86_64_general-obj)
	$(LD) -r $^ -o $@ 
