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

/*ssize_t read_until(int fd, void *buf, size_t count, char delimeter) {
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
}*/

#define MAX_SIZE 1024
struct Queue {
	char readSymbols[MAX_SIZE];
	char *startPointer, *endPointer;
};


char *__findInReadSymbols(struct Queue *queue, size_t *count, char delimeter) {
	for (; queue->startPointer != queue->endPointer && count != 0; ++queue->startPointer, --(*count)) {
		if (*queue->startPointer == delimeter) {
			return queue->startPointer++;
		}
	}
	return queue->endPointer;
}

void __ensureLimits(struct Queue *queue) {
	if (queue->endPointer == queue->readSymbols + MAX_SIZE) {
		char *pointer;
		for (pointer = queue->startPointer; pointer != queue->endPointer; ++pointer) {
			queue->readSymbols[pointer - queue->startPointer] = *pointer;
		}
		queue->endPointer = queue->readSymbols + (queue->endPointer - queue->startPointer);
		queue->startPointer = queue->readSymbols;
	}
}

void addInReadSymbols(struct Queue *queue, char symbol) {
	*(queue->endPointer++) = symbol;
	__ensureLimits(queue);
}

ssize_t read_until(int fd, void *buf, size_t count, char delimeter) {
	ssize_t readBytes = 0;
	static struct Queue *queue;
	queue = malloc(sizeof(struct Queue));
	queue->startPointer = queue->readSymbols;
	queue->endPointer = queue->readSymbols;
	while (count != 0) {
		char *pos = __findInReadSymbols(queue, &count, delimeter);
		readBytes += pos - queue->startPointer;
		if (pos == queue->endPointer) {
			queue->startPointer = queue->endPointer;
			int actuallyReadBytes = read(fd, buf, count);
			if (actuallyReadBytes == -1) {
				return -1;
			}
			if (actuallyReadBytes == 0) {
				break;
			}
			char *pointer;
			for (pointer = buf; pointer != buf + actuallyReadBytes; ++pointer) {
				if (*pointer == delimeter) {
					char *addPointer;
					for (addPointer = pointer + 1; addPointer != buf + actuallyReadBytes; ++addPointer) {
						addInReadSymbols(queue, *addPointer);
					}
					return readBytes + (pointer - buf + 1);
				}
			}
			count -= actuallyReadBytes;
		} else {
			queue->startPointer = pos + 1;
			return readBytes;
		}
	}
	return readBytes;
}
