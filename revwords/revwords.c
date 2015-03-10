#include "helpers.h"

const int MAX_BUFFER_SIZE = 1e5;

int main() {
	char buffer[MAX_BUFFER_SIZE];
	int almostReadBytes = 0;
	while (1) {
		int readBytes = read_until(STDIN_FILENO, buffer, MAX_BUFFER_SIZE, ' ');
		if (readBytes == 0) {
			break;
		}
		write_(STDOUT_FILENO, buffer, readBytes);
		almostReadBytes += readBytes + 1;
	}
}
