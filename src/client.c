#include "client.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>  
#include <errno.h>
#include <fcntl.h> 
#include <stddef.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define BUFFERSIZE 1024
char recvBuffer[BUFFERSIZE];
int recvBufferLen;

//创建TCP客户端
int createTCPClient(const char* addr, int port)
{
    struct sockaddr_in server_addr;
    int sockfd;
    /*create socket*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error!\n");
        return -1;
    }

    /*configure settings of connection*/
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(addr);

    /*connect*/
    int ret = connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(server_addr));
    if( ret < 0){
        printf("connected failed\n");
        return -1;
    }
    printf("connect success\n");
    //创建线程不断接受数据
    pthread_t pID;
    pthread_attr_t attr;
    struct sched_param param;
    param.sched_priority = 70;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);/*设置线程分离*/
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);//要使优先级其作用必须要有这句话
    if(pthread_create(&pID, &attr, TCPClientHandleThread, (void*)&sockfd) != 0)
        return -1;
    return 0;
}

void* TCPClientHandleThread(void *arg)
{
    int sockfd = *(int*)arg;
    while (1) {
        /*recv*/
        recvBufferLen = recv(sockfd, (void*)recvBuffer, BUFFERSIZE, 0);
        if(recvBufferLen < 0){
            if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
                usleep(1000);
            } else {
                printf("recv err\n");
                close(sockfd);
                break;
            }
        } else if(recvBufferLen == 0){
            printf("recv err\n");
            close(sockfd);
            break;
        } else {

            send(sockfd, "hello", sizeof(char) * 5, MSG_NOSIGNAL | MSG_DONTWAIT);
        }
        usleep(1000);
    }
}
