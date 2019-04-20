IDIR =./
ODIR =./obj
SDIR =./
CC=gcc
CFLAGS=-I$(IDIR)

VER=1
REV=0.0

_DEPS = Set.h \
		Object.h \
		new.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_BAGS_OBJ = main.o Bag.o
_SETS_OBJ = main.o Set.o
_LISTS_OBJ = main.o ListSet.o

BAGS_OBJ = $(patsubst %,$(ODIR)/%,$(_BAGS_OBJ))
SETS_OBJ = $(patsubst %,$(ODIR)/%,$(_SETS_OBJ))
LISTS_OBJ = $(patsubst %,$(ODIR)/%,$(_LISTS_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: sets bags lists

bags: $(BAGS_OBJ)
	$(CC) -o $@ $^

sets: $(SETS_OBJ)
	$(CC) -o $@ $^

lists: $(LISTS_OBJ)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o bags sets *~ core $(INCDIR)/*~
