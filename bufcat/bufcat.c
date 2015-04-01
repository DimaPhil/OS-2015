#include "bufio.h"

const int MAX_BUFFER_SIZE = 4096;

int main() {
	ssize_t readBytes = 0;
	struct buf_t *buffer = buf_new(MAX_BUFFER_SIZE);
	while (1) {
		readBytes = buf_fill(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);
		if (readBytes == -1 || readBytes == 0) {
			break;
		}
		buf_flush(STDOUT_FILENO, buffer, buf_size(buffer));
	}
	buf_flush(STDOUT_FILENO, buffer, buf_size(buffer));
	buf_free(buffer);
	return 0;
}
