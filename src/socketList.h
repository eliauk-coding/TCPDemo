#ifndef DW_SOCKETLIST_H
#define DW_SOCKETLIST_H

typedef struct stClientInfo
{
    int clientSocket;
    void *next;    
}ClientInfo;

int initSocketHead();

void appendSocket(int sockfd);

void deleteSocket(int sockfd);

void closeAllSocket();

void sendMsgToClient(const char* buf, int len);

#endif
