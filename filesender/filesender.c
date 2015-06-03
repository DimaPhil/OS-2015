#include "bufio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 65536

int main(int argc, char **argv) {
    struct addrinfo t;
    struct addrinfo *info, *nowInfo;
    int _socket, result;
    
    memset(&t, 0, sizeof(struct addrinfo));
    t.ai_family = AF_INET;
    t.ai_socktype = SOCK_STREAM;
    t.ai_flags = AI_PASSIVE;  
    t.ai_protocol = IPPROTO_TCP;
    
    result = getaddrinfo(0, argv[1], &t, &info);
    if (result != 0) {
        return 1;
    }

    nowInfo = info;
    while (nowInfo != 0) {
        _socket = socket(nowInfo->ai_family, nowInfo->ai_socktype, nowInfo->ai_protocol);
        if (_socket == -1) {
            nowInfo = nowInfo->ai_next;
            continue;
        }

        int status;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(int)) == -1) {
            return 1;
        }

        if (bind(_socket, nowInfo->ai_addr, nowInfo->ai_addrlen) == 0) {
            break;
        }

        close(_socket);
    }
    
    if (nowInfo == 0) { 
        return 1;
    }
    freeaddrinfo(info); 
  
    struct buf_t *buf = buf_new(MAX_BUFFER_SIZE);
    if (listen(_socket, -1) == -1) {
        return 1;
    }
    struct sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    while (1) { 
        int fd = accept(_socket, (struct sockaddr*)&client, &clientSize);
        if (fd == -1) {
            return 1;
        }
    
        if (fork() == 0) {    
            int file = open(argv[2], 0);
            if (file < 0) {
                return 1;
            }

            while (buf_fill(file, buf, buf->capacity) > 0) {
                buf_flush(fd, buf, buf->size);
            }
            close(fd);
            _exit(0);
        }
        close(fd);
    }
    close(_socket);
    return 0;
}
