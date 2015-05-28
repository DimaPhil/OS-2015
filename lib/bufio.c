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
        size_t oldSize = entry->size;
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
	return entry->size - oldSize;
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
	size_t pointer;
	for (pointer = 0; pointer != entry->size; ++pointer) {
		entry->buffer[pointer] = entry->buffer[pointer + writtenBytes];
	}
	return isAllWritten ? writtenBytes : -1;
}

size_t flushToDest(struct buf_t *entry, int length, char *dest, size_t alreadyWritten) {
    int pointer;
    for (pointer = 0; pointer != length; ++pointer) {
        dest[alreadyWritten + pointer] = entry->buffer[pointer];
    }
    size_t result = alreadyWritten + length;
    return result;
}

ssize_t buf_getline(int fd, struct buf_t *entry, char *dest) {
        #ifdef DEBUG
        if (entry == NULL) {
                abort();
        }
        #endif
        size_t readBytes = 0;
        size_t writtenBytes = 0;
        size_t i;
        for (i = 0; i != entry->size; ++i) {
                if (entry->buffer[i] == '\n') {
                        writtenBytes = flushToDest(entry, i, dest, writtenBytes);
                        size_t pointer;
                        for (pointer = i + 1; pointer != entry->size; ++pointer) {
                            entry->buffer[pointer - i - 1] = entry->buffer[pointer];
                        }
                        entry->size -= i + 1;
                        return readBytes;    
                }
                readBytes++;
        }
        while (1) {
            ssize_t addedBytes = buf_fill(fd, entry, 1);
            if (addedBytes == -1) {
                return -1;
            }
            if (addedBytes == 0) {
                writtenBytes = flushToDest(entry, entry->size, dest, writtenBytes);
                entry->size = 0;
                return readBytes;
            }
            int pointer;
            for (pointer = entry->size - addedBytes; pointer != entry->size; ++pointer) {
                if (entry->buffer[pointer] == '\n') {
                    writtenBytes = flushToDest(entry, pointer, dest, writtenBytes);
                    int ptr;
                    for (ptr = pointer + 1; ptr != entry->size; ++ptr) {
                        entry->buffer[ptr - pointer - 1] = entry->buffer[ptr];
                    }
                    size_t newBytes = pointer - (entry->size - addedBytes);
                    entry->size -= pointer + 1;
                    return readBytes + newBytes;
                }
            }
            if (entry->size == buf_capacity(entry)) {
                writtenBytes = flushToDest(entry, buf_capacity(entry), dest, writtenBytes);
                entry->size = 0;
            }
            readBytes += addedBytes;
        }
}

ssize_t buf_write(int fd, struct buf_t *entry, char *src, size_t srcLen) {
    size_t pointer = 0;
    size_t writtenBytes = 0;
    while (pointer != srcLen) {
        while (pointer != srcLen && entry->size != entry->capacity) {
            entry->buffer[entry->size++] = src[pointer++];
        }
        if (entry->size == entry->capacity) {
            ssize_t res = buf_flush(fd, entry, 1);
            if (res < 0) {
                return -1;
            }
            writtenBytes += res;
        }
    }
    return writtenBytes;
}
