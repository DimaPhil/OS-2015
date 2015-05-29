#include "helpers.h"
#include "bufio.h"

#define MAX_BUF_SIZE 4096
#define MAX_ARGUMENTS 2048

int spaceOrDelimeter(char c, char delimeter) {
    return c == delimeter || c == '\0' || c == ' ' || c == '\t';
}

int countLexems(char *buffer, char delimeter) {
    if (buffer[0] == '\0') {
        return 0;
    }
    int count = 0;
    size_t i = 1;
    while (1) {
        if (!spaceOrDelimeter(buffer[i - 1], delimeter) && spaceOrDelimeter(buffer[i], delimeter)) {
            count++;
        }
        if (buffer[i] == '\0' || buffer[i] == delimeter) {
            return count;
        }
        i++;
    }
}

char* getLexem(char *buffer, char *delimeter, size_t *newPosition) {
    size_t i = 0;
    while (buffer[i] == ' ' || buffer[i] == '\t') {
        i++;
    }
    size_t start = i;
    while (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\0' && buffer[i] != *delimeter) {
        i++;
    }
    size_t end = i;
    *delimeter = buffer[i];
    *newPosition = end;
    if (start == end) {
        return 0;
    }
    return strndup(buffer + start, end - start); 
}

size_t getDelimeter(char *buffer, char delimeter) {
    size_t i = 0; 
    while (buffer[i] == ' ' || buffer[i] == '\t') {
        i++;
    }
    if (buffer[i] == delimeter) {
        i++;
    }
    return i;
}

void sigint_handler(int signal) {
}

int main() {
    struct sigaction action;
    memset(&action, '\0', sizeof action);
    action.sa_handler = &sigint_handler;
   
    if (sigaction(SIGINT, &action, NULL) < 0) {
        return 1;
    }
 
    struct buf_t *entry = buf_new(MAX_BUF_SIZE); 
    while (1) {
        if (write_(STDOUT_FILENO, "$ ", 2) == -1) {
            return 1;
        }
        ssize_t position = buf_read_until(STDIN_FILENO, entry, '\n');
        if (position == -2) {
            if (write_(STDOUT_FILENO, "\n$ ", 3) == -1) {
                return 1;
            }
            continue;
        }
        if (position == -3) {
            return 0;
        }
        char *buffer = entry->buffer;        
        buffer[position] = '\0';
        struct execargs_t *arguments[MAX_ARGUMENTS];
        size_t argumentIndex = 0;
        while (1) {
            char delimeter = '|';
            int argc = countLexems(buffer, delimeter);
            if (argc == 0) {
                break;
            }
            char *argv[argc]; 
            size_t shift;
            int i;
            for (i = 0; i < argc; i++) {
                delimeter = '|';
                argv[i] = getLexem(buffer, &delimeter, &shift);
                buffer += shift;
            }
            arguments[argumentIndex] = (struct execargs_t*) malloc(sizeof(struct execargs_t));
            *arguments[argumentIndex] = getExecargs(argc, argv);  
            shift = getDelimeter(buffer, '|'); 
            buffer += shift;
            argumentIndex++;
        }
        entry->size -= (buffer - entry->buffer + 1);
        runpiped(arguments, argumentIndex);
    }
    return 0;
}
