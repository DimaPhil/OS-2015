#ifndef _HELPERS_H_
#define _HELPERS_H_

#define _GNU_SOURCE

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

ssize_t read_(int fd, void *buf, size_t count);
ssize_t write_(int fd, const void *buf, size_t count);
ssize_t read_until(int fd, void *buf, size_t count, char delimeter);
int spawn(const char *file, char *const argv[]);

struct execargs_t {
    char **argv;
};

struct execargs_t getExecargs(int argc, char **argv);
int exec(struct execargs_t *args);
int runpiped(struct execargs_t **programs, size_t n);

#endif
