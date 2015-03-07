COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lhelpers

all: cat

cat: cat.c
	$(COMPILER) cat.c $(COMPILER_FLAGS) -o cat

clean: 
	rm -f cat

