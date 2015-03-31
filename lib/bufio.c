#include "bufio.h"

struct buf_t *buf_new(size_t capacity) {
	struct buf_t *entry = malloc(sizeof(struct buf_t));
	if (entry == NULL) {
		return NULL;
	}
	entry->capacity = capacity;
	entry->size = 0;
	entry->buffer = malloc(sizeof(char) * capacity);
	if (entry->buffer == NULL) {
		free(entry);
		return NULL;
	}
	return entry;
}

void buf_free(struct buf_t *entry) {
	free(entry->buffer);
	free(entry);
}

size_t buf_capacity(struct buf_t *entry) {
	return entry->capacity;
}

size_t buf_size(struct buf_t *entry) {
	return entry->size;
}

ssize_t buf_fill(int fd, struct buf_t *entry, size_t required) {
	while (required > 0) {
		ssize_t actuallyReadBytes = read(fd, entry->buffer + entry->size, required);
		if (actuallyReadBytes == -1) {
			return -1;
		}
		if (actuallyReadBytes == 0) {
			break;
		}
		required -= actuallyReadBytes;
		entry->size += actuallyReadBytes;
	}
	return entry->size;
}

ssize_t buf_flush(int fd, struct buf_t *entry, size_t required) {
	size_t writtenBytes = 0;
	int allWritten = 1;
	while (required > 0 && writtenBytes < entry->size) {
		ssize_t actuallyWrittenBytes = write(fd, entry->buffer + writtenBytes, entry->size - writtenBytes);
		if (actuallyWrittenBytes == -1) {
			allWritten = 0;
			break;
		}
		required -= actuallyWrittenBytes;
		writtenBytes += actuallyWrittenBytes;
	}
	entry->size -= writtenBytes;
	int pointer;
	for (pointer = 0; pointer != entry->size; ++pointer) {
		entry->buffer[pointer] = entry->buffer[pointer + writtenBytes];
	}
	if (allWritten == 0) {
		return -1;
	}
	return writtenBytes;
}
