CC += -Wall
OBJS := ds.o int.o float.o string.o array.o hash.o demo.o
all: demo
clean:
	rm -f $(OBJS) demo
demo: $(OBJS)
	$(CC) $(CFLAGS) -o demo $(OBJS)
demo.o: demo.c ds.h
ds.o: ds.c ds.h
