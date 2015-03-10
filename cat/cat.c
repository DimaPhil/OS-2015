#include <stdio.h>
#include "helpers.h"

const int MAX_BUFFER_SIZE = 1000;

int main() {
    char buffer[MAX_BUFFER_SIZE];
    while (1) {
        int readBytes = read_(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);
		if (readBytes == 0) {
            break;
        }
		if (readBytes == -1) {
			fprintf(stderr, "An error occured while reading");
		}
        write_(STDOUT_FILENO, buffer, readBytes);
    }
}
