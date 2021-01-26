#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

#include "socketList.h"

ClientInfo* g_Head = NULL;

int initSocketHead()
{
    g_Head = (ClientInfo*)malloc(sizeof(ClientInfo) * sizeof(char));
    if(!g_Head){
        return -1;
    } else {
        g_Head->next = NULL;
        return 0;
    }
}

void appendSocket(int sockfd)
{
    ClientInfo* pHead = g_Head;
    ClientInfo* appendData = (ClientInfo*)malloc(sizeof(ClientInfo) * sizeof(char));

    while (pHead->next != NULL) {
        pHead = (ClientInfo*)pHead->next;
    }

    pHead->next = appendData;

    appendData->next = NULL;
    appendData->clientSocket = sockfd;
}

void deleteSocket(int sockfd)
{
    ClientInfo* pHead = g_Head;
    if(!pHead->next)
        return;
    while (pHead->next != NULL) {
        ClientInfo* psocket = (ClientInfo*)pHead->next;
        if(psocket->clientSocket == sockfd){
            pHead->next = (ClientInfo*)psocket->next;
            free(psocket);
            break;
        }
        pHead = (ClientInfo*)pHead->next;
    }
}

void closeAllSocket()
{
    ClientInfo* pHead = (ClientInfo*)g_Head->next;
    while (pHead != NULL) {
        int sockfd = pHead->clientSocket;
        deleteSocket(sockfd);
		close(sockfd);
        pHead = (ClientInfo*)pHead->next;
    }
}

void sendMsgToClient(const char* buf, int len)
{
    ClientInfo* pHead = (ClientInfo*)g_Head->next;
    while (pHead != NULL) {
        int sockfd = pHead->clientSocket;
        int num = send(sockfd, buf, sizeof(char) * len, MSG_NOSIGNAL | MSG_DONTWAIT);
        if(num < 0){
            printf("------ Client leave ! close socket ------\n");
            deleteSocket(sockfd);
			close(sockfd);
        }
        pHead = (ClientInfo*)pHead->next;
    }
}
