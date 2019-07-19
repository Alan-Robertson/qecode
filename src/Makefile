LIBDIR=../lib
SRCDIR=./src
OBJDIR=../build

LIBS=-lm

TARGETS=$(*.c)

CC = gcc
CFLAGS = -I$(LIBDIR) -O3
all: sym.o
.PHONY: all

%.o: %.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
