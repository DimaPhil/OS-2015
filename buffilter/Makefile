COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lbufio -lhelpers

all: buffilter

buffilter: buffilter.c
	$(COMPILER) buffilter.c $(COMPILER_FLAGS) -o buffilter

clean: 
	rm -f buffilter

