CC=gcc

strings	= strings.o new.o String.o
atoms	= strings.o new.o Atom.o
t		= strings atoms

all: $(t)
debug: CC += -g
debug: $(t)

strings: $(strings)
	$(CC) $(CFLAGS) -o $@ $^

atoms: $(atoms)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
