CC=gcc

sets	= main.o Set.o
bags	= main.o Bag.o
lists 	= main.o ListSet.o
t		= sets bags lists

all: $(t)
debug: CC += -g
debug: $(t)

sets: $(sets)
	$(CC) -o $@ $^

bags: $(bags)
	$(CC) -o $@ $^

lists: $(lists)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
