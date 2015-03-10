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

ssize_t write_(int fd, void *buf, size_t count) {
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
	int delimeterFound = 0;
	char *bufChar = (char*)buf;
	while (count != 0) {
		ssize_t actuallyReadBytes = read(fd, buf + readBytes, count);
		if (actuallyReadBytes == -1) {
			return -1;
		}
		if (actuallyReadBytes == 0) {
			break;
		}
		size_t index;
		for (index = readBytes; index < readBytes + actuallyReadBytes; index++) {
			if (bufChar[index] == delimeter) {
				readBytes = index;
				delimeterFound = 1;
				break;
			}
		}
		if (delimeterFound) {
			break;
		}
		count -= actuallyReadBytes;
		readBytes += actuallyReadBytes;
	}
	return readBytes;
}
