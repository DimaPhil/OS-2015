#include "helpers.h"

const int MAX_BUFFER_SIZE = 4096;

void processCommand(char *command, int argc, char *argv[], char *startPointer, char *endPointer) {
	char *pointer;
	for (pointer = startPointer; pointer != endPointer; ++pointer) {
		argv[argc - 1][pointer - startPointer] = *pointer;
	}
	argv[argc - 1][endPointer - startPointer] = 0;
	int exitCode = spawn(command, argv);
	if (exitCode == 0) {
		argv[argc - 1][endPointer - startPointer] = '\n';
		if (write_(STDOUT_FILENO, argv[argc - 1], endPointer - startPointer + 1) == -1) {
			perror("An error occurred while writing to STDOUT_FILENO");
			exit(0);
		}
	}
}

int main(int argc, char *argv[]) {
 	char *command = argv[1];
	char buffer[MAX_BUFFER_SIZE];
	int i;
	for (i = 1; i < argc; i++) {
		argv[i - 1] = argv[i];	
	}
	argv[argc - 1] = malloc(sizeof(char) * MAX_BUFFER_SIZE);
	char *endPointer = buffer;
	int readSymbolsCount = 0;
	while (1) {
		int readBytes = read_until(STDIN_FILENO, buffer + readSymbolsCount, MAX_BUFFER_SIZE - readSymbolsCount - 1, '\n');
		if (readBytes == -1) {
			perror("An error occurred while reading from STDIN_FILENO");
			return 0;
		}
		if (readBytes == 0) {
			break;
		}
		endPointer += readBytes;
		char *pointer;
		char *prevPointer = buffer;
		for (pointer = buffer; pointer != endPointer; ++pointer) {
			if (*pointer == '\n') {
				processCommand(command, argc, argv, prevPointer, pointer);
				prevPointer = pointer + 1;
			}
		}

		//moving to begin
		for (pointer = prevPointer; pointer != endPointer; ++pointer) {
			buffer[pointer - prevPointer] = *pointer;
		}
		readSymbolsCount = endPointer - prevPointer;
		endPointer = buffer + readSymbolsCount;
	}
	processCommand(command, argc, argv, buffer, endPointer);
	return 0;
}
