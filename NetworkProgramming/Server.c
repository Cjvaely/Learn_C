/*socket tcp服务器端*/
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
  
// #define SERVER_PORT 4800
#define BUFFER_LEN 1024
  
int main(int argc,char* argv[])
{  
    //调用socket函数返回的文件描述符  
    int sockfd;  
    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器  
    struct sockaddr_in server_addr;  
    struct sockaddr_in client_addr;  
    socklen_t addr_len = sizeof(client_addr);
    int connfd;
    char buffer[BUFFER_LEN];
    int n;

    // 判断终端参数为3个 ./Server ip ip_port
    if (argc != 3)
    {
        fputs("Usage: ./Server serverIP serverPORT\n", stderr);
        exit(1);
    }
    
    /**********************************************************************************  
    // 原型：int socket(int domain, int type, int protocol)
    // 参数：网络协议族，AF_INET(IPv4)和AF_INET6(IPv6); 协议类型，SOCK_STREAM(TCP)和SOCK_DGRAM(UDP); 特定的协议类型，默认为0
    // 返回值：失败返回-1，成功返回一个套接字描述符
    ***********************************************************************************/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {  
        perror("sockfd");  
        exit(1);
    }

    // 置字节字符串前n个字节为零且包括‘\0’，要置零的数据的起始地址，要置零的数据字节个数。
    bzero(&server_addr, sizeof(server_addr));
    // 若不清零，将导致sendto 出错：invalid argument
    bzero(&client_addr, sizeof(client_addr));

    // 指定地址族
    server_addr.sin_family = AF_INET;

    // 将端口和地址转成网络字节序，unsigned short类型
    // atoi将字符串转化成整数
    server_addr.sin_port = htons(atoi(argv[2]));

    /* 将端口和地址转成网络字节序，unsigned long类型
    ip可
    以是本服务器的ip，也可以用宏INADDR_ANY(0.0.0.0)代替，表明所有地址 */
    if (inet_aton(argv[1], &server_addr.sin_addr) == 0)
    {
        perror("inet_aton");
        exit(1);
    }

    /**********************************************************************************
    // 对于bind，accept之类的函数，里面套接字参数都是需要强制转换成(struct sockaddr*)  
    // bind三个参数：服务器端的套接字的文件描述符，
    // bind函数的用法：绑定IP地址和端口号到制定的套接字sock
    ***********************************************************************************/
    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)  
    {  
        perror("connect");  
        exit(1);
    }
    // 设置被接受的TCP连接请求的最大数量5，并监听请求
    // 成功返回0，失败返回-1
    if(listen(sockfd, 5) < 0)   
    {  
        perror("listen");  
        exit(1);
    }
    
    printf("Listening on port: %d\n", atoi(argv[2]));

    // 循环，收发数据
    while(1)
    {
        /**********************************************************************************
        原型：int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
        参数：套接字描述符；获得对方的套接字地址；套接字地址的长度
        调用后，进入阻塞状态，接受连接请求，成功返回一个非负整数作为连接套接字描述符，否则为-1
        ***********************************************************************************/
        connfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addr_len);  
        if(connfd < 0)  
        {  
            perror("accept");
            exit(1);
        }
        memset(buffer, 0, BUFFER_LEN);
        n = recvfrom(connfd, buffer, BUFFER_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);
        if(n < 0)  
        {  
            perror("fail to receive");  
            exit(1);
        }
        else
        {
            // 设置接收字符串的结束符
            buffer[n] = '\0';
            if(strcmp(buffer, "exit") == 0)
            {
                break;
            }
            else
            {
                printf("receive %d words is: %s\n", n, buffer);
                n = sendto(connfd, buffer, n, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
                if(n == -1)
                {
                    perror("Fail to reply ");
                    close(connfd);
                    exit(1);
                }
            }          
        }   
    }
    if(close(sockfd) == -1)
    {
        perror("close sockfd");
        exit(1);
    }
    puts("TCP Server is closed!\n");
    return 0;
} 
