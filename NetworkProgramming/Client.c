/*socket tcp客户端*/  
#include <stdio.h>              // 标准输入输出 perror函数
#include <string.h>             // 字符数组相关函数 bzero函数 strcmp
#include <stdlib.h>             // 标准库
#include <sys/types.h>          // 基本系统数据类型
#include <sys/socket.h>         // socket函数 bind函数 地址数据结构
#include <sys/stat.h> 
#include <netinet/in.h>         // struct sockaddr_in结构所在头文件
#include <arpa/inet.h>          // ip转换函数 inet_ntoa函数
#include <unistd.h>             // 系统调用接口，close函数
#include <errno.h>              // 通过错误码来回报错误资讯的宏

#define BUFFER_LEN 1024
  
int main(int argc,char* argv[])
{ 
    //客户机只需要一个套接字文件描述符，用于和服务机通信
    int sockfd;
    int con;
    struct sockaddr_in serverAddr;
    socklen_t addr_len = sizeof(serverAddr);
    char buffer[BUFFER_LEN];    
    int n;

    // 判断终端参数为3个 ./Client ip ip_port
    if (argc != 3)
    {
        fputs("Usage: ./Server serverIP serverPORT\n", stderr);
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {
        perror("sockfd");  
        return -1;  
    }
    printf("sockfd:%d\n",sockfd);
	
	// 置字节字符串前n个字节为零且包括‘\0’，要置零的数据的起始地址，要置零的数据字节个数。
    bzero(&serverAddr, sizeof(serverAddr));

  	// 设置地址族
    serverAddr.sin_family = AF_INET;
    // 设置端口号
    serverAddr.sin_port = htons(atoi(argv[2]));
    // 设置ip 地址
    if (inet_aton(argv[1], &serverAddr.sin_addr) == 0)
    {
        perror("inet_aton");
        exit(1);
    }

    /**********************************************************************************
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    建立套接字和给定服务器地址之间的连接
    如果sockfd是TCP描述符，则建立一条连接；否则只建立和addr的绑定关系
    成功返回0，失败返回-1
    ***********************************************************************************/
    con = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if( con < 0 )  
	{
    	perror("connect");
    	return -1;  
	}

    printf("connect with destination host...\n");

    // 清空缓存，收发数据
	memset(buffer, 0, BUFFER_LEN);
    printf("Input your world:>");
    gets(buffer);
    printf("\n");
    n = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (n == -1)
    {
    	perror("Fail to send");
    	exit(1);
    }
    else
    {
    	if(strcmp(buffer, "exit") == 0)
    	{
        	exit(0);
    	}
	    else
	    {
	    	memset(buffer, 0, BUFFER_LEN);
	    	n = recvfrom(sockfd, buffer, BUFFER_LEN, 0, (struct sockaddr *)&serverAddr, &addr_len);
		    if (n == -1)
		    {
		    	perror("Fail to receive");
		    	exit(1);
		    }
		    else
		    {
		    	buffer[n] = '\0';
		    	printf("Receive data: %s\n", buffer);
		    }
	    }
    }
    if( close(sockfd) == -1)
    {
    	perror("fail to close");
    	exit(1);
    }
    puts("TCP Client is closed!\n");
    return 0;  
}  
