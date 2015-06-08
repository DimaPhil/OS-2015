COMPILER = gcc
COMPILER_FLAGS = -g -L../lib -I../lib -Wl,-rpath=../lib -lhelpers -lbufio

all: forking polling

forking: forking.c
	$(COMPILER) forking.c $(COMPILER_FLAGS) -o forking

polling: polling.c
	$(COMPILER) polling.c $(COMPILER_FLAGS) -o polling

clean: 
	rm -f forking
	rm -f polling
