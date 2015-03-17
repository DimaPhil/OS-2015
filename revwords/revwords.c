#include "helpers.h"

const int MAX_BUFFER_SIZE = 1e5;

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
	int almostReadBytes = 0;
	while (1) {
		int readBytes = read_until(STDIN_FILENO, buffer, MAX_BUFFER_SIZE, ' ');
		if (readBytes == -1) {
			perror("An error occured while reading from STDIN_FILENO");
		}
		if (readBytes == 0) {
			break;
		}
		if (buffer[readBytes - 1] != ' ') {
			reverse(buffer, buffer + readBytes);
			write_(STDOUT_FILENO, buffer, readBytes);
		} else {
			reverse(buffer, buffer + readBytes - 1);
			write_(STDOUT_FILENO, buffer, readBytes - 1);
			write_(STDOUT_FILENO, buffer + readBytes - 1, 1);
		}
	}
}
