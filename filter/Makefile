COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lhelpers

all: filter

filter: filter.c
	$(COMPILER) filter.c $(COMPILER_FLAGS) -o filter

clean: 
	rm -f filter
