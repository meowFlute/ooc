CC=gcc

points	= points.o new.o Point.o
circles	= circles.o new.o Point.o Circle.o
t		= points circles

all: $(t)
debug: CC += -g
debug: $(t)

points: $(points)
	$(CC) -o $@ $^

circles: $(circles)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f ./*.o $(t) *~ core $(INCDIR)/*~
