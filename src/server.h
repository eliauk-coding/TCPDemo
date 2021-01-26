#ifndef DW_TCPSERVER_H
#define DW_TCPSERVER_H

#include <sys/epoll.h>

#define MAX_EVENT_NUMBER 1024

typedef struct stServerInfo{
    int listenfd;
    int epollfd;
    struct epoll_event events[ MAX_EVENT_NUMBER ];
}ServerInfo;

int setnonblocking( int fd );

void addfd( int epollfd, int fd );

//创建TCP服务端
int createTCPServer(int port);

void* TCPServerHandleThread(void *arg);
/*broadcast data*/
void* BroadcastData(void *arg);
#endif
