COMPILER = gcc
COMPILER_FLAGS = -O0 -Wall -Werror

all: libhelpers.so

clean:
	rm -f *.o
	rm -f *.so

libhelpers.so: helpers.o
	$(COMPILER) -shared -o libhelpers.so helpers.o
	
helpers.o: helpers.c
	$(COMPILER) $(COMPILER_FLAGS) -c -fpic helpers.c
