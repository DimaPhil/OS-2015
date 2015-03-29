#include <unistd.h>
#include <stdio.h>

#define DEBUG

struct buf_t {
	size_t capacity;
	size_t size;
	char *buffer;
};

struct buf_t *buf_new(size_t capacity);
void buf_free(struct buf_t*); 
size_t buf_capacity(buf_t*);
size_t buf_size(buf_t*);
ssize_t buf_fill(fd_t fd, buf_t *buf, size_t required);
