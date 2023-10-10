# Makefile for compiling all C files in the current directory

CC = gcc
OBJS = $(wildcard *.c)

all: $(OBJS)
	$(CC) $(OBJS)

# Rule for building each object file
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean target
clean:
	rm -f *.o

# Run tests
test: clean all
	./a.out
