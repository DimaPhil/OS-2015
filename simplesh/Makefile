COMPILER = gcc
COMPILER_FLAGS = -g -L../lib -I../lib -Wl,-rpath=../lib -lhelpers -lbufio

all: simplesh

simplesh: simplesh.c
	$(COMPILER) simplesh.c $(COMPILER_FLAGS) -o simplesh

clean: 
	rm -f simplesh
