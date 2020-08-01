INCLUDE=project.h
CFLAGS=-Wall -std=c99 -pedantic -g -include $(INCLUDE)
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

PROG=das

all: $(PROG)
das: elf.o

clean:
	rm -rf $(PROG) $(OBJS)
