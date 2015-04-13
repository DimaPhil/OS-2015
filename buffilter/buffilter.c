#include "bufio.h"
#include "helpers.h"

const size_t MAX_BUFFER_SIZE = 4096;

void processCommand(char *command, int argc, char *argv[], char *startPointer, char *endPointer, struct buf_t *buffer) {
	char *pointer;
	for (pointer = startPointer; pointer != endPointer; ++pointer) {
		argv[argc - 1][pointer - startPointer] = *pointer;
	}
	argv[argc - 1][endPointer - startPointer] = 0;
	int exitCode = spawn(command, argv);
	if (exitCode == 0) {
		argv[argc - 1][endPointer - startPointer] = '\n';
		if (buf_write(STDOUT_FILENO, buffer, argv[argc - 1], endPointer - startPointer + 1) == -1) {
			perror("An error occurred while writing to STDOUT_FILENO");
			exit(0);
		}
	}
}

int main(int argc, char *argv[]) {
 	char *command = argv[1];
        char dest[MAX_BUFFER_SIZE];
	struct buf_t *buffer = buf_new(MAX_BUFFER_SIZE);
	struct buf_t *resultBuffer = buf_new(MAX_BUFFER_SIZE);
        int i;
	for (i = 1; i < argc; i++) {
		argv[i - 1] = argv[i];	
	}
	argv[argc - 1] = malloc(sizeof(char) * MAX_BUFFER_SIZE);
        size_t readBytes;
	while (1) {
		readBytes = buf_getline(STDIN_FILENO, buffer, dest);
                dest[readBytes] = '\0';
		if (readBytes == -1) {
			perror("An error occurred while reading from STDIN_FILENO");
			return 0;
		}
		if (readBytes == 0) {
			break;
		}
                processCommand(command, argc, argv, dest, dest + readBytes, resultBuffer);
	}
        buf_flush(STDOUT_FILENO, resultBuffer, buf_size(resultBuffer));
	return 0;
}
