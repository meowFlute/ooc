CC=gcc

postfix	= parse.o postfix.o
value	= parse.o value.o
infix	= parse.o infix.o
t	= postfix value infix

all: $(t)
debug: CC += -g
debug: $(t)

postfix: $(postfix)
	$(CC) -o $@ $^

value: $(value)
	$(CC) -o $@ $^

infix: $(infix)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
