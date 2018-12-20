// Lab3 Server端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>

// 服务器端口号
#define MYPORT 1234
// listen队列中等待的连接数
#define BACKLOG 5
// 缓冲区大小
#define BUF_SIZE 1024

int main(void)
{
    int i, n, maxfd;
    int nready;
    // select所需的文件描述符集合 
    fd_set rset, allset;
	// socket文件描述符
    int listenfd, connectfd;
    //FD_SETSIZE为select函数支持的最大描述符个数
    int client[BACKLOG];

    // 声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    // 地址信息结构体大小
    socklen_t sin_size;
    sin_size = sizeof(client_addr);
    
    // 交互信息缓冲区
    char buf[BUF_SIZE];
    // 服务器端标准输入
    char stdbuf[BUF_SIZE];

    // 创建初始套接字
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Create socket failed");
        exit(1);
    }
    
    // 初始化服务器端地址
	bzero(&server_addr, sizeof(server_addr));
	// 定义服务器端的协议IPv4
    server_addr.sin_family = AF_INET;
    // 初始化服务器端的套接字，并用htons和htonl将端口和地址转成网络字节序
    server_addr.sin_port = htons(MYPORT);
    // INADDR_ANY表示允许任何IP地址
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //设置socket属性   
    int opt = SO_REUSEADDR;  
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定监听套接字，出错则显示出错信息并退出
    if (bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind error");
        exit(1);
    }
    
    // 监听客户端
    if (listen(listenfd, BACKLOG) < 0)
    {
        perror("listen() error");
        exit(1);
    }
    printf("listen on port %d\n", MYPORT);

    //初始化select  
    maxfd = listenfd;  
    // 初始化连接套接字集合
    memset(&client, -1, sizeof(client));
    // 清空集合
    FD_ZERO(&allset);
    //将监听socket加入select检测的描述符集合
    FD_SET(listenfd, &allset);
    // 将标准输入套接字加入select检测的描述符集合
    FD_SET(0, &allset);
    while (1)
    {
        rset = allset;
        //调用select   
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0)
        {
            perror("Select ");
            exit(1);
        }
        // 标准输入监听
	    if (FD_ISSET(0, &rset))
        {
            // 给stdbuf分配空间接收输入
            memset(stdbuf, 0, BUF_SIZE);
            // 接受标准输入
            gets(stdbuf);
            // 若输入为exit则退出所有客户端服务器并关闭所有连接
            if (strcmp(stdbuf, "exit") == 0)
            {
                // 给客户端发送消息，并关闭所有客户端连接
                for (i = 0; client[i] != -1 && i < BACKLOG; i++)
                {
                    send(client[i], stdbuf, sizeof(stdbuf), 0);
                    close(client[i]);
                }
                // 关闭监听套接字
                if (close(listenfd) == -1)
                {
                    perror("Close listensocket failed ");
                    exit(1);
                }
                // 成功退出
                return EXIT_SUCCESS;
            }
        }
        // 动态添加监听的套接字并加入rset集合
        if (FD_ISSET(listenfd, &rset))
        {
            // 接受客户端的连接请求，返回连接套接字
            connectfd = accept(listenfd, (struct sockaddr *)&client_addr, &sin_size);
            if (connectfd < 0)
            {
                perror("accept");
                exit(1);
            }
            // 查找 client 数组中是否还有空余位置
            for (i = 0; i < BACKLOG; i++)
            {
                // 如果集合有空位，则将套接字放进并加入allset集合
                if (client[i] == -1)
                {
                    client[i] = connectfd;
                    FD_SET(connectfd, &allset);
                    // 更新maxfd
                    if (connectfd > maxfd)
                    {
                        maxfd = connectfd;
                    }
                    printf("Client[%d] join.\n", client[i]);
                    break;
                }
                // 查找不到，告诉客户端已经满了
                if (i == (BACKLOG-1))
                {
                    strcpy(buf, "queue has been full");
                    send(connectfd, buf, sizeof(buf), 0);
                    close(connectfd);
                }
            }
        }
	    // 创建多个客户端文件描述符
	    for (i = 0; i < BACKLOG; i++)
	    {
            //如果客户端描述符小于0，则没有客户端连接，检测下一个
            if (client[i] == -1)
            {
                continue;
            }
            // 有客户连接，检测是否有数据
            if (FD_ISSET(client[i], &rset))
            {
                printf("Receive from connect client %d.\n", client[i]);
                n = recv(client[i], buf, BUF_SIZE, 0);
                if (n == -1)
                {
                    perror("Fail to receive");
                    exit(1);
                }
                // 如果接收到客户端发送的exit
                if (strcmp(buf, "exit") == 0)
                {
                    // 关闭连接
                    close(client[i]);
                    // 清出allset集合
                    FD_CLR(client[i], &allset);
                    printf("Client %d exit\n", client[i]);
                    // 数组位变为未占用
                    client[i] = -1;
                }
                else
                {
                    // 处理文本尾部
                    buf[n] = '\0';
                    // 显示文本
                    printf("The message of client %d is: %s\n", client[i], buf);
                    // 发送信息给客户端
                    if (send(client[i], buf, sizeof(buf), 0) == -1)
                    {
                        perror("Fail to reply");
                        exit(1);
                    }
                }
            }
	    }
    }
}