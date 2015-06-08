#define _GNU_SOURCE

#include "bufio.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

#define MAX_BUF_SIZE 65536

int getSocket(const char * port) {
    struct addrinfo hints;
    struct addrinfo *info, *nowInfo;
    int _socket, result;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;       
    hints.ai_protocol = IPPROTO_TCP;
   
    result = getaddrinfo(NULL, port, &hints, &info);
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

        int one;
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
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

#define MAX_FD_COUNT 256

int main(int argc, char **argv) {
    struct pollfd fds[MAX_FD_COUNT];
    memset(&fds, 0, sizeof(struct pollfd) * MAX_FD_COUNT);

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = SIG_IGN;  
    action.sa_flags = SA_RESTART; 
   
    if (sigaction(SIGCHLD, &action, NULL) < 0) {
        return 1;
    }
    
    int socket1 = getSocket(argv[1]);
    int socket2 = getSocket(argv[2]);
  
    if (listen(socket1, -1) == -1 || listen(socket2, -1) == -1) {
        return 1;
    }
    
    struct sockaddr_in client1;
    struct sockaddr_in client2;
    socklen_t clientSize1 = sizeof(client1);
    socklen_t clientSize2 = sizeof(client2);
  
    size_t cnt = 0;
    struct buf_t *buffer1[127];
    struct buf_t *buffer2[127];
    fds[0].fd = socket1;
    fds[0].events = POLLIN;
    fds[1].fd = socket2;
    fds[1].events = POLLIN;
   
    int state = 0;
    int fd1, fd2;

    while (1) { 
        int res = poll(fds, 2 + 2 * cnt, -1);
        if (res <= 0 && errno != EINTR) {
            return 1;
        }

        size_t i;
        for (i = 0; i < cnt; i++) {         
            if ((fds[2 + 2 * i].revents & POLLRDHUP) ||
                (fds[2 + 2 * i + 1].revents & POLLRDHUP)) {
                close(fds[2 + 2 * i].fd);
                close(fds[2 + 2 * i + 1].fd);
                buf_free(buffer1[i]);
                buf_free(buffer2[i]);
                buffer1[i] = 0;
                buffer2[i] = 0;
                
                struct pollfd tmp = fds[2 + 2 * i];
                fds[2 + 2 * i] = fds[2 + 2 * cnt - 2];
                fds[2 + 2 * cnt - 2] = tmp;
                tmp = fds[2 + 2 * i + 1];
                fds[2 + 2 * i + 1] = fds[2 + 2 * cnt - 1];
                fds[2 + 2 * cnt - 1] = tmp;
                struct buf_t *tmp2 = buffer1[i];
                buffer1[i] = buffer1[cnt - 1];
                buffer1[cnt - 1] = tmp2;
                tmp2 = buffer2[i];
                buffer2[i] = buffer2[cnt - 1];
                buffer2[cnt - 1] = tmp2;

                --cnt;
                fds[0].events |= POLLIN;
                fds[1].events |= POLLIN;
                break;
            }
            if (fds[2 + 2 * i].revents & POLLIN) {
                buf_fill(fds[2 + 2 * i].fd, buffer1[i], 1);
                fds[2 + 2 * i + 1].events |= POLLOUT;
                if (buffer1[i]->size == buffer1[i]->capacity) {
                    fds[2 + 2 * i].events &= ~POLLIN;
                }
            }
            if (fds[2 + 2 * i + 1].revents & POLLIN) {
                buf_fill(fds[2 + 2 * i + 1].fd, buffer2[i], 1);
                fds[2 + 2 * i].events |= POLLOUT;
                if (buffer2[i]->size == buffer2[i]->capacity) {
                    fds[2 + 2 * i + 1].events &= ~POLLIN;
                }
            }
            if (fds[2 + 2 * i].revents & POLLOUT) {
                buf_flush(fds[2 + 2 * i].fd, buffer2[i], 1);
                fds[2 + 2 * i + 1].events |= POLLIN;
                if (buffer2[i]->size == 0) {
                    fds[2 + 2 * i].events &= ~POLLOUT;
                }
            }
            if (fds[2 + 2 * i + 1].revents & POLLOUT) {
                buf_flush(fds[2 + 2 * i + 1].fd, buffer1[i], 1);
                fds[2 + 2 * i].events |= POLLIN;
                if (buffer1[i]->size == 0) {
                    fds[2 + 2 * i + 1].events &= ~POLLOUT;
                }
            }            
        }

        if (state == 0 && (fds[0].revents & POLLIN)) {        
            fd1 = accept(socket1, (struct sockaddr*)&client1, &clientSize1);
            if (fd1 == -1) {
                return 1;
            }
            state = 1;
        }
        if (state == 1 && (fds[1].revents & POLLIN)) {
            fd2 = accept(socket2, (struct sockaddr*)&client2, &clientSize2);
            if (fd2 == -1) {
                return 1;
            }
            state = 0;
            fds[2 + 2 * cnt].fd = fd1;
            fds[2 + 2 * cnt].events = POLLIN | POLLRDHUP;
            fds[2 + 2 * cnt + 1].fd = fd2;
            fds[2 + 2 * cnt + 1].events = POLLIN | POLLRDHUP;
            buffer1[cnt] = buf_new(MAX_BUF_SIZE);
            buffer2[cnt] = buf_new(MAX_BUF_SIZE);
            if (++cnt >= 127) {
                fds[0].events &= ~POLLIN;
                fds[1].events &= ~POLLIN;
            }
        }
   
    }

    close(socket1);
    close(socket2);
    return 0;
}
