CC = gcc
CFLAGS = -pthread

all: mutex_stack spinl_stack treib_stack tr_bo_stack

%: main.c %.c stack.h clock_constant.h
	$(CC) $(CFLAGS) -o $@ $^ -g

clock_constant.h:
	./gen_clock_header.sh


clean:
	rm -f *_stack
