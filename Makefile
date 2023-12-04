CC = gcc
CFLAGS = -pthread

all: mutex_stack spinl_stack treib_stack

%: main.c %.c
	$(CC) $(CFLAGS) -o $@ $^



clean:
	rm -f main
