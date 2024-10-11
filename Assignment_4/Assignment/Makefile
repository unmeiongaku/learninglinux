.PHONY: all clean counter

CC := gcc
CFLAGS := -pthread

all:
	$(CC) -o app main.c $(CFLAGS)
counter:
	$(CC) -o counter counter.c $(CFLAGS)
clean:
	rm -rf counter
	rm -rf app
	rm -rf *.txt
