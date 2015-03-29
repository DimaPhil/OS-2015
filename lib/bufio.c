#include "bufio.h"

struct buf_t *buf_new(size_t capacity) {
	struct buf_t *buffer = malloc(sizeof(buf_t));
	if (buffer == NULL) {
		return NULL;
	}
	buffer->capacity = capacity;
	buffer->size = 0;
	return buffer;
}

void buf_free(struct buf_t *buffer) {
	free(buffer);
}

size_t buf_capacity(buf_t *buffer) {
	return buffer->capacity;
}

size_t buf_size(buf_t *buffer) {
	return buffer->size;
}

ssize_t buf_fill(fd_t fd, buf_t *buf, size_t required) {
	ssize_t readBytes = 0;
	while (required != 0) {
		//int actuallyReadBytes = read(fd, buf + readBytes, required);
	}
	return readBytes;
}
