CC += -Wall
OBJS := ds.o demo.o
all: demo
clean:
	rm -f $OBJS demo
demo: $OBJS
demo.o: demo.c ds.h
ds.o: ds.c ds.h
