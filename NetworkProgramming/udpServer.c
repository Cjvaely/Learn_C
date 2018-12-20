/*socket udp服务器端*/
#include <stdio.h>              // 标准输入输出 perror函数
#include <string.h>             // 字符数组相关函数 bzero函数 strcmp
#include <stdlib.h>             // 标准库
#include <sys/types.h>          // 基本系统数据类型
#include <sys/socket.h>         // socket函数 bind函数 地址数据结构
#include <netinet/in.h>         // struct sockaddr_in结构所在头文件
#include <arpa/inet.h>          // ip转换函数 inet_ntoa函数
#include <unistd.h>             // 系统调用接口，close函数
#include <errno.h>              // 通过错误码来回报错误资讯的宏

#define BUFFER_LEN 1024

int main(int argc, char *argv[])  
{
    // 调用socket函数返回的文件描述符  
    int sockfd;
    // 声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器  
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_LEN];  
    int n;
    
    // 判断终端参数为3个 ./Server ip ip_port
    if (argc != 3)
    {
        fputs("Usage: ./Server serverIP serverPORT\n", stderr);
        exit(1);
    }  

    //创建套接字，失败返回-1，IPV4 UDP协议
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  
    {
        perror("create udp sockfd");  
        exit(1);
    }
    // 套接字地址信息结构体变量初始化清零
    bzero(&server_addr, sizeof(server_addr));  
    // 指定地址族: AF_INET（IPv4）
    server_addr.sin_family = AF_INET;
    // htons将端口地址转成网络字节序
    server_addr.sin_port = htons(atoi(argv[2]));  
    // 设置IP地址
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
    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)  
    {
        perror("Bind");  
        exit(1);  
    }

    // 循环收发数据
    while(1)  
    {
    	// 清空buffer存储空间
    	memset(buffer, 0, BUFFER_LEN);
        // 阻塞I/O方式从网络接收数据,数据源在client_addr
        n = recvfrom(sockfd, buffer, BUFFER_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);

        // 接收出错
        if(n < 0)
        {
            perror("fail receive");
            continue;
        }
        else
        {
            // 加‘\0’处理，结束符，防止乱码
            buffer[n] = '\0';

            // 如果收到exit，服务器退出程序
            if(strcmp(buffer, "exit") == 0)
            {
                break;
            }
            else
            {
                // 显示收到的文本信息
                printf("Receive %d words is: %s\n", n, buffer);
                n = sendto(sockfd, buffer, n, 0, (struct sockaddr*)&client_addr, addr_len);
                if (n == -1)
                {
                    perror("fail to reply");
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
    puts("UDP Server is closed!\n");
    return 0;
}
