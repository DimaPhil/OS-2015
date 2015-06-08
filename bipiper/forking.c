#include "bufio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_BUFFER_SIZE 65536

int getSocket(const char *port) {
    struct addrinfo hints;
    struct addrinfo *info, *nowInfo;
    int _socket;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;           
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; 
    hints.ai_protocol = IPPROTO_TCP;
   
    int result = getaddrinfo(0, port, &hints, &info);
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
    return _socket;
}

int main(int argc, char **argv) {
    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = SIG_IGN; 
    action.sa_flags = SA_RESTART;
   
    if (sigaction(SIGCHLD, &action, 0) < 0) {
        return 1;
    }
    
    int socket1 = getSocket(argv[1]);
    int socket2 = getSocket(argv[2]);
  
    struct buf_t *buf = buf_new(MAX_BUFFER_SIZE);
    if (listen(socket1, -1) == -1 || listen(socket2, -1) == -1) {
        return 1;
    }
    
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    socklen_t clientSize1 = sizeof(client1);
    socklen_t clientSize2 = sizeof(client2);
   
    while (1) { 
        int fd1 = accept(socket1, (struct sockaddr*)&client1, &clientSize1);
        if (fd1 == -1) {
            return 1;
        }
        int fd2 = accept(socket2, (struct sockaddr*)&client2, &clientSize2);
        if (fd2 == -1) {
            return 1;
        }
   
        if (fork() == 0) {    
            while (buf_fill(fd1, buf, 1) > 0) {
                buf_flush(fd2, buf, buf->size);
            }
            _exit(0);
        }
        
        if (fork() == 0) {    
            while (buf_fill(fd2, buf, 1) > 0) {
                buf_flush(fd1, buf, buf->size);
            }
            _exit(0);
        }

        close(fd1);
        close(fd2);
    }

    close(socket1);
    close(socket2);
    return 0;
}
