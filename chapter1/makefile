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

_BAGS_OBJ = main.o \
			Bag.o
_SETS_OBJ = main.o \
			Set.o

BAGS_OBJ = $(patsubst %,$(ODIR)/%,$(_BAGS_OBJ))
SETS_OBJ = $(patsubst %,$(ODIR)/%,$(_SETS_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: sets bags

bags: $(BAGS_OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

sets: $(SETS_OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o bags sets *~ core $(INCDIR)/*~
