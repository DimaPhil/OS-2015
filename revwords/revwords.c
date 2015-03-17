#include "helpers.h"

const int MAX_BUFFER_SIZE = 1024;
const char DELIMETER = ' ';

void reverse(char *start, char *end) {
	char *iteratorStart = start, *iteratorEnd = end - 1;
	for (; iteratorStart < iteratorEnd; ++iteratorStart, --iteratorEnd) {
		char tmp = *iteratorStart;
		*iteratorStart = *iteratorEnd;
		*iteratorEnd = tmp;
	} 
}

int main() {
	char buffer[MAX_BUFFER_SIZE];
	char readSymbols[MAX_BUFFER_SIZE * 2];
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
				reverse(startPointer, endPointer);
				if (write_(STDOUT_FILENO, startPointer, endPointer - startPointer) == -1 || write_(STDOUT_FILENO, endPointer, 1) == -1) {
					perror("An error occured while writing to STDOUT_FILENO");
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
	reverse(readSymbols, readSymbols + readSymbolsCount);
	if (write_(STDOUT_FILENO, readSymbols, readSymbolsCount) == -1) {
		perror("An error occured while writing to STDOUT_FILENO");
	}
}
