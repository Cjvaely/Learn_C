/*select tcp客户端*/  
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>  
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
  
#define BUF_SIZE 1024 
  
int main(int argc,char* argv[])
{ 
    //客户机只需要一个套接字文件描述符，用于和服务机通信  
    int clientSocket;  
    //描述服务器的socket  
    struct sockaddr_in serverAddr;
    // socklen_t addr_len = sizeof(serverAddr);
    char sendbuf[BUF_SIZE];  
    char recvbuf[BUF_SIZE];  
    int iDataNum;

    if (argc != 3)
    {
        fputs("Usage: ./Server serverIP serverPORT\n", stderr);
        exit(1);
    }

    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {
        perror("socket");  
        return -1;  
    }
    printf("clientSocket:%d\n",clientSocket);
  
    serverAddr.sin_family = AF_INET;
    // 服务器端口号
    int SERVER_PORT;
    sscanf(argv[2], "%d", &SERVER_PORT);
    serverAddr.sin_port = htons(SERVER_PORT);   // SERVER_PORT 
    //指定服务器端的ip，本地测试：127.0.0.1  
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    // 建立套接字和给定服务器地址之间的连接
    int con = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  	printf("connect:%d",con);
    if( con < 0)  
    {
        perror("connect");
        return -1;  
    }  
  
    printf("connect with destination host...\n");  
  
    while(1)  
    {  
        printf("Input your world:>");  
        gets(sendbuf);
        printf("\n");  
  
        send(clientSocket, sendbuf, strlen(sendbuf), 0);  
        if(strcmp(sendbuf, "exit") == 0)
        {
            break;
        }  
        iDataNum = recv(clientSocket, recvbuf, BUF_SIZE, 0);  
        recvbuf[iDataNum] = '\0';  
        printf("received data of my world is: %s\n", recvbuf);  
    }  
    close(clientSocket);  
    return 0;  
}  

