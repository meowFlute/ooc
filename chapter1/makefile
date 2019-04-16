IDIR =./
ODIR =./obj
SDIR =./
CC=gcc
CFLAGS=-I$(IDIR)

VER=1
REV=0.0

_DEPS = Set.h \
		# Object.h \
		# new.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = 	Chapter1.o \
		Set.o \
		# Object.o \
		# new.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Chapter1: $(OBJ)
	$(CC) -o $@ $< $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o chapter1 *~ core $(INCDIR)/*~
