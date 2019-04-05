#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

/******** 宏定义 *********/
#define BUFFSIZE 100

int main()
{
    int sockFd;
    int n;
    struct sockaddr_in serverAddr;
    char sendBuf[BUFFSIZE], recvBuf[BUFFSIZE];

    //创建套接字
    if((sockFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        exit(1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    //连接套接字
    if(connect(sockFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }

    //从标准输入中读取一行数据
    while(fgets(sendBuf, BUFFSIZE, stdin) != NULL)
    {
        //将数据写入到套接字，发送给服务器
        write(sockFd, sendBuf, strlen(sendBuf));

        //从套接字中读取从服务器发来的数据
        n = read(sockFd, recvBuf, BUFFSIZE);

        //将接受到的数据写到标准输出中
        write(STDIN_FILENO, recvBuf, n);
    }

    return 0;
}
