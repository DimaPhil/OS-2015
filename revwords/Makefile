COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lhelpers

all: revwords

revwords: revwords.c
	$(COMPILER) revwords.c $(COMPILER_FLAGS) -o revwords

clean: 
	rm -f revwords
