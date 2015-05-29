#include "helpers.h"
#include "bufio.h"

#define MAX_BUF_SIZE 4096
#define MAX_ARGUMENTS 2048

int isWhiteSpace(char c) {
    return c == ' ';
}

int isDelimeter(char c, char delimeter) {
    return c == '\0' || c == delimeter;
}

int isSpaceOrDelimeter(char c, char delimeter) {
    return isDelimeter(c, delimeter) || isWhiteSpace(c);
}

int countCommands(char *buffer, char delimeter) {
    if (buffer[0] == '\0') {
        return 0;
    }
    int words = 0;
    size_t i;
    for (i = 1;; i++) {
        if (!isSpaceOrDelimeter(buffer[i - 1], delimeter) && isSpaceOrDelimeter(buffer[i], delimeter)) {
            words++;
        }
        if (isDelimeter(buffer[i], delimeter)) {
            return words;
        }
    }
}

char* nextCommand(char *buffer, char *delimeter, size_t *newPos) {
    size_t i = 0;
    while (isWhiteSpace(buffer[i])) {
        i++;
    }
    size_t startPos = i;
    while (!isWhiteSpace(buffer[i]) && buffer[i] != '\0' && buffer[i] != *delimeter) {
        i++;
    }
    size_t endPos = i;
    *delimeter = buffer[i];
    *newPos = endPos;
    if (startPos == endPos) {
        return 0;
    }

    char *result = (char*)malloc((endPos - startPos + 1) * sizeof(char));
    for (i = 0; i < endPos - startPos; i++) {
        result[i] = buffer[startPos + i];
    }
    return result;
}

size_t nextDelimeterPos(char *buffer, char delimeter) {
    size_t i = 0; 
    while (isWhiteSpace(buffer[i])) {
        i++;
    }
    if (buffer[i] == delimeter) {
        i++;
    }
    return i;
}

void sigintHandler(int signal) {
}

int main() {
    struct sigaction action;
    memset(&action, '\0', sizeof action);
    action.sa_handler = &sigintHandler;
   
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
            int commands = countCommands(buffer, delimeter);
            if (commands == 0) {
                break;
            }
            char *argv[commands]; 
            size_t newPos = 0;
            int i;
            for (i = 0; i < commands; i++) {
                argv[i] = nextCommand(buffer, &delimeter, &newPos);
                delimeter = '|';
                buffer += newPos;
            }
            arguments[argumentIndex] = (struct execargs_t*) malloc(sizeof(struct execargs_t));
            *arguments[argumentIndex] = getExecargs(commands, argv);  
            newPos = nextDelimeterPos(buffer, '|'); 
            buffer += newPos;
            argumentIndex++;
        }
        entry->size -= (buffer - entry->buffer + 1);
        runpiped(arguments, argumentIndex);
    }
    return 0;
}
