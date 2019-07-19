CC = gcc
CFLAGS = -O3

LIBDIR=lib
SRCDIR=src



all: sym.o
.PHONY: all

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS) 


.PHONY: clean

