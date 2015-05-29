#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//#define DEBUG

struct buf_t {
	size_t capacity;
	size_t size;
	char *buffer;
};

struct buf_t *buf_new(size_t capacity);
void buf_free(struct buf_t*); 
size_t buf_capacity(struct buf_t*);
size_t buf_size(struct buf_t*);
ssize_t buf_fill(int fd, struct buf_t *entry, size_t required);
ssize_t buf_flush(int fd, struct buf_t *entry, size_t required);
ssize_t buf_read_until(int fd, struct buf_t *entry, char delimeter);
