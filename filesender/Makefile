COMPILER = gcc
COMPILER_FLAGS = -g -L../lib -I../lib -Wl,-rpath=../lib -lhelpers -lbufio

all: filesender

filesender: filesender.c
	$(COMPILER) filesender.c $(COMPILER_FLAGS) -o filesender

clean: 
	rm -f filesender
