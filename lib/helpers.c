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
    size_t writtenBytes = 0;
    while (count != 0) {
        ssize_t actuallyWrittenBytes = write(fd, buf + writtenBytes, count);
        if (actuallyWrittenBytes == 0) {
            break;
        }
        count -= actuallyWrittenBytes;
		writtenBytes += actuallyWrittenBytes;
    }
    return writtenBytes;
}
