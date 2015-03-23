#include "helpers.h"

const int MAX_BUFFER_SIZE = 1024;
const char DELIMETER = ' ';
const int MAX_LENGTH_SIZE = 100;

int main() {
	char buffer[MAX_BUFFER_SIZE];
	char readSymbols[MAX_BUFFER_SIZE * 2];
	char lengthRepresentation[MAX_LENGTH_SIZE];
	int readSymbolsCount = 0;
	while (1) {
		int readBytes = read_until(STDIN_FILENO, buffer, MAX_BUFFER_SIZE, DELIMETER);
		if (readBytes == -1) {
			perror("An error occured while reading from STDIN_FILENO");
			return 0;
		}
		if (readBytes == 0) {
			break;
		}
		int pos;
		for (pos = readSymbolsCount; pos != readSymbolsCount + readBytes; ++pos) {
			readSymbols[pos] = buffer[pos - readSymbolsCount];
		}
		readSymbolsCount += readBytes;
		char *startPointer = readSymbols;
		char *endPointer = readSymbols;
		for (; endPointer != readSymbols + readSymbolsCount; ++endPointer) {
			if (*endPointer == DELIMETER) {
				//printLength(startPointer, endPointer);
				sprintf(lengthRepresentation, "%d", endPointer - startPointer);
				if (write_(STDOUT_FILENO, lengthRepresentation, strlen(lengthRepresentation)) == -1 || write_(STDOUT_FILENO, endPointer, 1) == -1) {
					perror("An error occurred while writing to STDOUT_FILENO");
					return 0;
				}
				startPointer = endPointer + 1;
			}
		}

		//moving to the begin
		char *pointer;
		for (pointer = startPointer; pointer != endPointer; ++pointer) {
			readSymbols[pointer - startPointer] = *pointer;
		}
		readSymbolsCount = endPointer - startPointer;
		startPointer = readSymbols;
		endPointer = startPointer + readSymbolsCount;
	}
	if (readSymbolsCount > 0) {
		sprintf(lengthRepresentation, "%d", readSymbolsCount);
		if (write_(STDOUT_FILENO, lengthRepresentation, strlen(lengthRepresentation)) == -1) {
			perror("An error occurred while writing to STDOUT_FILENO");
			return 0;
		}
	}
}

