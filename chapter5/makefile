CC=gcc
LFLAGS=-lm

value	= binary.o Name.o value.o Var.o mathlib.o parse.o
t		= value

all: $(t)
debug: CC += -g
debug: $(t)

value: $(value)
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
