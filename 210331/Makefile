CFLAGS= -static -Wall -O3 
CROSS_COMPILE = arm-linux-

CC      =$(CROSS_COMPILE)gcc #--verbose
STRIP   =$(CROSS_COMPILE)strip
LD      =$(CROSS_COMPILE)ld #-m elf32arm26
AR      =$(CROSS_COMPILE)ar
RANLIB  =$(CROSS_COMPILE)ranlib

CPP		= $(CC) -E
NM		= $(CROSS_COMPILE)nm
DEP		= $(CROSS_COMPILE)gcc
CPP		= $(CROSS_COMPILE)g++
ARFLAGS = rv
LINKER  = $(CROSS_COMPILE)g++ --verbose


CFLAGS += -I./libhdmi/include -I./libhdmi

default: 210329\ 02

clean:
	rm -f 210329\ 02 *.o

210329\ 02: mouse.c
	$(CC) mouse.c -o mouse -L. -lhdmi -lm $(CFLAGS) -L./libhdmi
