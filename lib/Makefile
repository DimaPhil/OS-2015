COMPILER = gcc
COMPILER_FLAGS = -O0 -Wall -Werror

all: libhelpers.so libbufio.so

clean:
	rm -f *.o
	rm -f *.so

libhelpers.so: helpers.o
	$(COMPILER) -shared -o libhelpers.so helpers.o
	
helpers.o: helpers.c
	$(COMPILER) $(COMPILER_FLAGS) -c -fpic helpers.c

libbufio.so: bufio.o
	$(COMPILER) -shared -o libbufio.so bufio.o

bufio.o: bufio.c
	$(COMPILER) $(COMPILER_FLAGS) -c -fpic bufio.c
