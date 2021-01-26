#ifndef CLIENT_H
#define CLIENT_H

//创建TCP客户端
int createTCPClient(const char* addr, int port);

/*客户端接受出来任务*/
void* TCPClientHandleThread(void *arg);

#endif
