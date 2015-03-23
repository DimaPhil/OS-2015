COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lhelpers

all: lenwords

lenwords: lenwords.c
	$(COMPILER) lenwords.c $(COMPILER_FLAGS) -o lenwords

clean: 
	rm -f lenwords
