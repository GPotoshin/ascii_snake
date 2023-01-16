LD_FLAGS+=-lncurses
CC_FLAGS+=-O2

ifeq ($(test -d bin && echo 1 || echo 0), 1)
	ALL := bin/snake
else
	ALL := bin bin/snake
endif

all: $(ALL)

bin:
	mkdir -p bin

bin/snake: src/main.c
	$(CC) $< $(CC_FLAGS) $(LD_FLAGS) -o $@
