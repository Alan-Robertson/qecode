LIBDIR=../lib
SRCDIR=./src
OBJDIR=../build
EIGDIR=./Eigen/src

LIBS=-lm

TARGETS=$(*.c)

CC = gcc
CFLAGS = -I$(LIBDIR) -I$(EIGDIR) -O3
all: sym.o
.PHONY: all

%.o: %.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
