CC=gcc

strings	= strings.o new.o String.o
atoms	= strings.o new.o Atom.o
t		= strings atoms

all: $(t)
debug: CC += -g
debug: $(t)

strings: $(strings)
	$(CC) -o $@ $^

atoms: $(atoms)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
