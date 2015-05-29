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

ssize_t findDelimeter(char *buffer, size_t len, char delimeter) {
    size_t i;
    for (i = 0; i < len; i++) {
        if (buffer[i] == '\0') {
            break; 
	}
        if (buffer[i] == delimeter) {
            return i;
	}
    }
    return -1;
}

ssize_t buf_read_until(int fd, struct buf_t *entry, char delimeter) {
    int result;
    ssize_t position;
    while (1) {
	position = findDelimeter(entry->buffer, entry->size, delimeter);
	if (position != -1) {
	    break;
	}
        result = read(fd, entry->buffer + entry->size, entry->capacity - entry->size);
        if (result == 0) {
            break;
	}
        if (result > 0) {
            entry->size += result;
        } else { 
            return -2; 
        }
    }
    if (result == 0) {
        return -3;
    }
    return position;
}
