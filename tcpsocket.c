#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>


#define BUFFSIZE 100

int main()
{
    int listenFd, connectFd;
    socklen_t len;
    int n;
    struct sockaddr_in srcAddr, cliAddr;
    char buf[BUFFSIZE];

    //create socket
//    函数原型
//    int socket(int family,int type,int protocol); 　　　
//  　　 　返回：非负描述字－－－成功　　　-1－－－失败
//第一个参数指明了协议簇，目前支持5种协议簇，最常用的有AF_INET(IPv4协议)和AF_INET6(IPv6协议)；
//第二个参数指明套接口类型，有三种类型可选：SOCK_STREAM(字节流套接口， TCP)、SOCK_DGRAM(数据报套接口，UDP)
//和SOCK_RAW(原始套接口)；如果套接口类型不是原始套接口，那么第三个参数就为0。


    if((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket error");
        exit(1);
    }

    bzero(&srcAddr, sizeof(srcAddr));
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_port = htons(8888);
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//    通配地址实现：htonl(INADDR_ANY)
//    通配地址，内核将等到套接字已连接TCP或已经发出数据报(UDP)时才指定。

    //绑定套接字
    if(bind(listenFd, (struct sockaddr*)&srcAddr, sizeof(srcAddr)) == -1)
    {
        perror("bind error");
        exit(1);
    }

    //监听套接字
    if(listen(listenFd, 5) == -1)
    {
        perror("listen error");
        exit(1);
    }

    len = sizeof(cliAddr);

    printf("listening ...\n");

    while(1)
    {
        //从已完成队列中抓取一个套接字进行服务
//        accept函数从已完成连接的队列中取走一个套接字，如果该队列为空，则accept函数阻塞。
//        accept函数的返回值称为已连接套接字，已连接的套接字就建立一个完整的TCP连接，
//        源IP地址，源端口号，目的IP地址，目的端口号都是唯一确定了。
        if((connectFd = accept(listenFd, (struct sockaddr*)&cliAddr, &len)) == -1)
        {
            perror("accept error");
            exit(1);
        }

        printf("accepted successful\n");

        //从套接字中读取从客户端发来的数据
        while((n = read(connectFd, buf, BUFFSIZE)) > 0)
        {
            if(buf[n] != '\0')
            {
                buf[n] = '\0';
            }
            printf("receive a message: %s", buf);

            //将读取的数据写进套接字，发给客户端
            if(write(connectFd, buf, n) < 0)
            {
                perror("write error");
                exit(1);
            }
        }
    }

    //进程退出后，所有打开的文件描述符都会被关闭，因此打开的套接字文件也被关闭
    return 0;
}
