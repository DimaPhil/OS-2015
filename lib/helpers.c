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

ssize_t read_until(int fd, void *buf, size_t count, char delimeter) {
	ssize_t readBytes = 0;
	while (count != 0) {
		ssize_t actuallyReadBytes = read(fd, buf + readBytes, count);
		if (actuallyReadBytes == -1) {
			return -1;
		}
		if (actuallyReadBytes == 0) {
			break;
		}

		int foundDelimeter = 0;
		char *pointer = buf + readBytes;
		int counter;
		for (counter = 0; counter != actuallyReadBytes; ++counter, ++pointer) {
			if (*pointer == delimeter) {
				foundDelimeter = 1;
				break;
			}
		}
		count -= actuallyReadBytes;
		readBytes += actuallyReadBytes;
		if (foundDelimeter) {
			return readBytes;
		}
	}
	return readBytes;
}

int spawn(const char *file, char *const argv[]) {
	pid_t child = fork();
	if (child == -1) {
		return -1;
	} else if (child > 0) {
		int status;
		waitpid(child, &status, 0);
		return status;
	} else {
		exit(execvp(file, argv));
	}
}

struct execargs_t getExecargs(int argc, char **argv)
{
    struct execargs_t result;
    result.argv = (char**)malloc((argc + 1) * sizeof(char*));
    int i;
    for (i = 0; i < argc; i++) {
        result.argv[i] = strdup(argv[i]); 
    }
    result.argv[argc] = NULL;
    return result;
}

int exec(struct execargs_t *args) {
    if (spawn(args->argv[0], args->argv) < 0) {
        return -1;
    }
    return 0;
}
 
size_t childrenCount;
int *children;

void killChildren() {
    size_t i;
    for (i = 0; i < childrenCount; i++) {
        kill(children[i], SIGKILL);
    }
    childrenCount = 0;
}

int runpiped(struct execargs_t **programs, size_t n) {
    if (n == 0) {
        return 0;
    }
    int pipefd[n - 1][2];
    int child[n];	       
    size_t i;
    for (i = 0; i + 1 < n; i++) {
        if (pipe2(pipefd[i], O_CLOEXEC) < 0) {
            return -1;
        }
    }
    for (i = 0; i < n; i++) {
        child[i] = fork();
	if (child[i] == 0) {
            if (i != 0) {
		dup2(pipefd[i - 1][0], STDIN_FILENO);
            }
            if (i != n - 1) {
	        dup2(pipefd[i][1], STDOUT_FILENO);
            }
	    _exit(execvp(programs[i]->argv[0], programs[i]->argv));	
	}
        if (child[i] == -1) {
            return -1;
        }
    }
    for (i = 0; i + 1 < n; i++) {
	close(pipefd[i][0]);
	close(pipefd[i][1]);
    }
    
    childrenCount = n;
    children = (int*)child;
    struct sigaction act;
    memset(&act, '\0', sizeof act);
    act.sa_handler = &killChildren;
   
    if (sigaction(SIGINT, &act, NULL) < 0)  {
        return -1;
    }

    int status;
    for (i = 0; i < n; i++) {
        waitpid(children[i], &status, 0);
    }
    childrenCount = 0;
    return 0;
}
