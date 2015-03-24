#include "helpers.h"

ssize_t read_(int fd, void *buf, size_t count) {
	ssize_t readBytes = 0;
	while (count != 0) {
		ssize_t actuallyReadBytes = read(fd, buf + readBytes, count);
		if (actuallyReadBytes == -1) {
			return -1;
		}
		if (actuallyReadBytes == 0) {
			break;
		}
		count -= actuallyReadBytes;
		readBytes += actuallyReadBytes;
	}
	return readBytes;
}

ssize_t write_(int fd, const void *buf, size_t count) {
	ssize_t writtenBytes = 0;
	while (count != 0) {
		ssize_t actuallyWrittenBytes = write(fd, buf + writtenBytes, count);
		if (actuallyWrittenBytes == -1) {
			return -1;
		}
		count -= actuallyWrittenBytes;
		writtenBytes += actuallyWrittenBytes;
	}
	return writtenBytes;
}

ssize_t read_until(int fd, void *buf, size_t count, char delimeter) {
	ssize_t readBytes = 0;
	while (count != 0) {
		ssize_t actuallyReadBytes = read(fd, buf + readBytes, count);
		if (actuallyReadBytes == -1) {
			return -1;
		}
		if (actuallyReadBytes == 0) {
			break;
		}

		int foundDelimeter = 0;
		char *pointer = buf + readBytes;
		int counter;
		for (counter = 0; counter != actuallyReadBytes; ++counter, ++pointer) {
			if (*pointer == delimeter) {
				foundDelimeter = 1;
				break;
			}
		}
		count -= actuallyReadBytes;
		readBytes += actuallyReadBytes;
		if (foundDelimeter) {
			return readBytes;
		}
	}
	return readBytes;
}

int spawn(const char *file, char *const argv[]) {
	pid_t child = fork();
	if (child == -1) {
		return -1;
	} else if (child > 0) {
		int status;
		waitpid(child, &status, 0);
		return status;
	} else {
		exit(execvp(file, argv));
	}
}

