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
	#ifdef DEBUG
	if (entry == NULL) {
		abort();
	}
	#endif
	free(entry->buffer);
	free(entry);
}

size_t buf_capacity(struct buf_t *entry) {
	#ifdef DEBUG
	if (entry == NULL) {
		abort();
	}
	#endif
	return entry->capacity;
}

size_t buf_size(struct buf_t *entry) {
	#ifdef DEBUG
	if (entry == NULL) {
		abort();
	}
	#endif
	return entry->size;
}

ssize_t buf_fill(int fd, struct buf_t *entry, size_t required) {
	#ifdef DEBUG
	if (entry == NULL || entry->capacity < required) {
		abort();
	}
	#endif
	while (required != 0) {
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
	#ifdef DEBUG
	if (entry == NULL) {
		abort();
	}
	#endif
	size_t writtenBytes = 0;
	int isAllWritten = 1;
	while (required != 0 && writtenBytes < entry->size) {
		ssize_t actuallyWrittenBytes = write(fd, entry->buffer + writtenBytes, entry->size - writtenBytes);
		if (actuallyWrittenBytes == -1) {
			isAllWritten = 0;
			break;
		}
		if (required >= actuallyWrittenBytes) {
			required -= actuallyWrittenBytes;
		} else {
			required = 0;
		}
		writtenBytes += actuallyWrittenBytes;
	}
	entry->size -= writtenBytes;
	int pointer;
	for (pointer = 0; pointer != entry->size; ++pointer) {
		entry->buffer[pointer] = entry->buffer[pointer + writtenBytes];
	}
	return isAllWritten ? writtenBytes : -1;
}
