#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include "server.h"
#include "socketList.h"

//服务端连接客户端个数限制
#define MAXCLIENTNUM 5

#define BUFFERSIZE 1024
int sendBuffer[BUFFERSIZE];
int sendBufferLen;

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

void addfd( int epollfd, int fd )
{
    struct epoll_event event;
    event.data.fd = fd;
    //event.events = EPOLLIN | EPOLLET;
    event.events = EPOLLIN;//可读事件，默认为LT模式，事件一般被触发多次
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

int createTCPServer(int port)
{
    int ret = 0;
    
    //创建tcp套接字，并绑定 、监听
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    address.sin_port = htons( port );
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    ServerInfo server_info;

    server_info.listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( server_info.listenfd >= 0 );

    int option = 1;
    setsockopt(server_info.listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    ret = bind( server_info.listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );

    ret = listen( server_info.listenfd, MAXCLIENTNUM );
    assert( ret != -1 );
    
    // struct epoll_event events[ MAX_EVENT_NUMBER ];
    server_info.epollfd = epoll_create( MAXCLIENTNUM );
    assert( server_info.epollfd != -1 );
    //注册tcp
    addfd( server_info.epollfd, server_info.listenfd );

    initSocketHead();

    //创建线程不断接受数据
    pthread_t pID1, pID2;
    pthread_attr_t attr;
    struct sched_param param;
    param.sched_priority = 70;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);/*设置线程分离*/
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话

    if(pthread_create(&pID1, &attr, TCPServerHandleThread, (void*)&server_info) != 0)
        return -1;
    if(pthread_create(&pID2, &attr, BroadcastData, (void*)&server_info) != 0)
        return -1;
    return 0;
}

void* TCPServerHandleThread(void *arg)
{
    ServerInfo server_info = *(ServerInfo*)arg;
    while(1)
    {
        int number = epoll_wait( server_info.epollfd, server_info.events, MAX_EVENT_NUMBER, -1 );
        if ( number < 0 ){
            printf( "epoll failure\n" );
            break;
        }
        for ( int i = 0; i < number; i++ ) {
            int sockfd = server_info.events[i].data.fd;
            //tcp有新的可读事件，也即接受到了新的连接
            if ( sockfd == server_info.listenfd ) {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( server_info.listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                printf("server get connection from %s %d\n", inet_ntoa(client_address.sin_addr), connfd);
                //将新的连接套接字也注册可读事件
                addfd( server_info.epollfd, connfd );
                appendSocket(connfd);
                
            } else if ((server_info.events[i].events & EPOLLIN) && (server_info.events[i].events & EPOLLRDHUP)){
                printf("Epoll Event Error!\n");
                close (server_info.events[i].data.fd);  
                continue; 
            } else if(server_info.events[i].events & EPOLLOUT){
                //int num = send(sockfd, gBuffPointer.tcpSendBuffer, gBuffPointer.tcpBufferLen, MSG_NOSIGNAL | MSG_DONTWAIT); 
                int num = send(server_info.events[i].data.fd, "ping", 4, MSG_NOSIGNAL  | MSG_DONTWAIT); 
                if(num < 0){
                     printf("------ Client leave ! close socket ------\n");
			         close(sockfd);
                }
            }
        }
        usleep(50 * 1000);
    }
    close( server_info.listenfd );
    return NULL;
}

void* BroadcastData(void *arg)
{
    while (1)
    {
        /*broadcast data*/
        sendMsgToClient("hello client", 12);
        usleep(1000 * 1000);
    } 
}
