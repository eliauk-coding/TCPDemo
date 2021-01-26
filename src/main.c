#include <stdio.h>
#include <unistd.h> 
#include <string.h>

#include "server.h"
#include "client.h"

#define REMOTE_IP "192.168.0.174"
#define REMOTE_PORT 2055
#define LISTEN_PORT 2066

int main(int argc, char **argv) { 
    //初始化TCP客户端和服务端
    printf("start init TCP Client...\n");
    if(createTCPClient(REMOTE_IP, REMOTE_PORT) == -1)
        return -1;
    printf("init TCP Client success\n");
    printf("start init TCP Server...\n");
    if(createTCPServer(LISTEN_PORT) == -1)
        return -1;
    printf("init TCP Server success\n");
    while (1){}
    
    return 0;
}
