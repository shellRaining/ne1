# Makefile for compiling all C files in the current directory

CC = gcc
OBJS = $(wildcard *.c)
OUTDIR = ../bin
PROG = server
CFLAGS = -I../include
LIBS = $(wildcard ../lib/*.c)

all: $(OBJS)
	$(CC) $(OBJS) -o $(OUTDIR)/$(PROG) $(CFLAGS) $(LIBS)

# Clean target
clean:
	rm -f *.o a.out

# Run tests
test: all
	$(OUTDIR)/$(PROG)
