COMPILER = gcc
COMPILER_FLAGS = -L../lib -I../lib -Wl,-rpath=../lib -lbufio

all: bufcat

bufcat: bufcat.c
	$(COMPILER) bufcat.c $(COMPILER_FLAGS) -o bufcat

clean: 
	rm -f bufcat

