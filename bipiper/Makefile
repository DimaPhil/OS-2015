COMPILER = gcc
COMPILER_FLAGS = -g -L../lib -I../lib -Wl,-rpath=../lib -lhelpers -lbufio

all: bipiper

bipiper: forking.c
	$(COMPILER) forking.c $(COMPILER_FLAGS) -o forking

clean: 
	rm -f bipiper
