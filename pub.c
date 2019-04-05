// 在windows上编译加#define WIN
#define WIN
#ifdef WIN
#include <winsock2.h>
//#pragma comment( lib, "ws2_32.lib" )
#else
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<errno.h>
#include<string.h>
#include <fcntl.h>
#define SOCKET int
#define INVALID_SOCKET -1
#endif

#define ERRORCODE -1
#include<stdio.h>
#define BUFFSIZE 10240

void get_filename(char *name, char *filename)
{
	int len;
	char *tmp = NULL;
	if (name == NULL)
		return;
	len = strlen(name);
	tmp = name + len - 1; //指向字符串末端
	while (tmp != name && *tmp != '\\' && *tmp != '/')
	{
		tmp--;
	} // 可能给的路径下的文件
	if(tmp == name)
	{
		strcpy(filename, name);
	}
	else
	{
		strcpy(filename, tmp + 1);
	}
}
int init_socket()
{
#ifdef WIN
    WSADATA wsaData;
    int Ret;
   // Initialize Winsock version 2.2
	if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		if (WSACleanup() == SOCKET_ERROR)
		{
			printf("WSACleanup failed with error %d\n", WSAGetLastError());
		}
		return ERRORCODE;
	}
   // When your application is finished call WSACleanup
#endif
	return 0;
}
SOCKET socket_connect(char *ip_str, int i_port)
{
	SOCKET st;
	struct sockaddr_in client_sockaddr; //定义IP地址结构
	int i_connect_rv;
	if (init_socket() == -1)
	{
		printf("init socket fail! \n");
		return INVALID_SOCKET;
	}
	st = socket(AF_INET, SOCK_STREAM, 0); //建立socket
	if (st == INVALID_SOCKET)
	{
		printf("%s,%d:create socket error!",__FILE__,__LINE__);
		return INVALID_SOCKET;
	}

	memset(&client_sockaddr, 0, sizeof(client_sockaddr));
	client_sockaddr.sin_port = htons(i_port); //指定一个端口号并将hosts字节型传化成Inet型字节型（大端或或者小端问题）
	client_sockaddr.sin_family = AF_INET;	//设置结构类型为TCP/IP
	client_sockaddr.sin_addr.s_addr = inet_addr(ip_str);
	i_connect_rv = connect(st, (struct sockaddr*) &client_sockaddr,
			sizeof(client_sockaddr));
	if (i_connect_rv == -1)
	{
		printf("%s,%d:connect error!",__FILE__,__LINE__);
		return  INVALID_SOCKET;
	}

	return st;
}
int send_work(char *ip_str, int i_port, char *filename)
{
	int rv =0;
	FILE *fp = NULL ;
	char buf[BUFFSIZE];
	SOCKET st = socket_connect(ip_str, i_port);
	if (st == INVALID_SOCKET)
	{
		return ERRORCODE;
	}
	printf("connect success,start send file! \n");


	memset(buf, 0, BUFFSIZE);

	get_filename(filename, buf);

	fp = fopen(filename, "rb");//这里没有考虑文件名相同的情况
	if(fp ==NULL)
	{
		printf("%s,%d:open file error! \n",__FILE__,__LINE__);
		return ERRORCODE;
	}

	printf("send file: %s..... \n", buf); //打印出文件名
	/* 将文件名发给服务器，然后等待服务器反馈后在发送文件内容，
	否则客户端发送的文件名和文件内容混在一起，无法区分 */
	rv = send(st, buf, strlen(buf), 0);

	if (rv <= 0)
	{
		printf("%s,%d:send filename error! \n",__FILE__,__LINE__);
		return ERRORCODE;
	}
	else
    {
        printf("send filename success!!\n");
    }
	memset(buf, 0, BUFFSIZE);
	rv = recv(st, buf, BUFFSIZE, 0);//接收服务器的反馈 即OK
	if (rv <= 0)
	{
		printf("%s,%d:recv error! \n",__FILE__,__LINE__);
		return ERRORCODE;
	}
	else
    {
        printf("客户端收到服务端接收文件名成功的反馈！\n");
    }
	//收到来自服务端的ok 后开始发送文件内容


	printf("start send file....\n");
	if (strncmp(buf, "OK", 2) == 0)
	{
		while (1)
		{
			memset(buf, 0, BUFFSIZE);
			rv = fread(buf, 1, BUFFSIZE, fp);
			if (rv <= 0)// 等于0说明文件读完了
			{
				if (rv < 0)
				{
					printf("%s,%d:fread file error! \n",__FILE__,__LINE__);
					return ERRORCODE;
				}
				break;
			}
			rv = send(st, buf, rv, 0);
			if (rv <= 0)
			{
				printf("%s,%d:send error! \n",__FILE__,__LINE__);
				return ERRORCODE;
			}
		}
	}
// 释放资源
#ifdef WIN
	closesocket(st);
	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("WSACleanup failed with error %d\n", WSAGetLastError());
		return ERRORCODE;
	}

#else
	close(st);
#endif
	if (fp)
	{
		fclose(fp);
	}
	return 0;
}

SOCKET create_listensocket(int i_port)
{
	SOCKET listen_socket;
	struct sockaddr_in sockaddr; //定义IP地址结构
	//struct sockaddr_in clinet_sockaddr; //定义client IP地址结构
	if (init_socket() == -1)
	{
		printf("%s,%d:init socket error!",__FILE__,__LINE__);
		return INVALID_SOCKET;
	}

	listen_socket = socket(AF_INET, SOCK_STREAM, 0);  //初始化socket
	if (listen_socket == INVALID_SOCKET)
	{
		printf("%s,%d:socket create error!",__FILE__,__LINE__);
		return INVALID_SOCKET;
	}

	memset(&sockaddr, 0, sizeof(sockaddr));

	sockaddr.sin_port = htons(i_port); //指定一个端口号并将hosts字节型传化成Inet型字节型（大端或或者小端问题）
	sockaddr.sin_family = AF_INET;	//设置结构类型为TCP/IP
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//服务端是等待别人来连，不需要找谁的ip
	//这里写一个长量INADDR_ANY表示server上所有ip，这个一个server可能有多个ip地址，因为可能有多块网卡
	if (bind(listen_socket, (struct sockaddr *) &sockaddr, sizeof(sockaddr))
			== -1)
	{
		printf("%s,%d:recv error!",__FILE__,__LINE__);
		return INVALID_SOCKET; //
	}
	if(listen(listen_socket,10)== -1) //bind 和listen出错
	{
		printf("%s,%d:listen error! \n",__FILE__,__LINE__);
		return INVALID_SOCKET;
	}
	return listen_socket;
}

SOCKET accept_socket(SOCKET listen_socket) // 获取连接来的客户端
{
	struct sockaddr_in clinet_sockaddr;
	SOCKET client_socket;
	//windows 和linx 下accept函数最后一个参数有些区别
    //这里使用条件编译下
#ifdef WIN
	int len = 0;
#else
	socklen_t len = 0;
#endif
	len = sizeof(struct sockaddr);
	memset(&clinet_sockaddr, 0, sizeof(clinet_sockaddr));
	client_socket = accept(listen_socket,
			(struct sockaddr*) &clinet_sockaddr, &len);
	if (client_socket == INVALID_SOCKET)
	{
		printf("%s,%d:accpet error! \n",__FILE__,__LINE__);
		return INVALID_SOCKET;
	}
	return client_socket;
}
int recv_work(int i_port) // 服务端接收文件
{
	char buf[BUFFSIZE];
	FILE *fp = NULL;
	int rv = 0;
	SOCKET client_socket ;
	SOCKET listen_socket = create_listensocket(i_port);

	if (listen_socket == INVALID_SOCKET)
	{
		return ERRORCODE;
	}
	printf("listen success port: %d \n",i_port);

	while(1)
    {
        client_socket = accept_socket(listen_socket);
        if (client_socket == INVALID_SOCKET)
        {
            return ERRORCODE;
        }

        memset(buf, 0, BUFFSIZE);
        rv = recv(client_socket, buf, BUFFSIZE, 0);//获取文件名
        if (rv <= 0)
        {
            printf("%s,%d:recv error! \n",__FILE__,__LINE__);
            return ERRORCODE;
        }
        fp = fopen(buf, "wb");
        if(fp == NULL)
        {
            printf("%s,%d:fopen file error! \n",__FILE__,__LINE__);
            return ERRORCODE;
        }
        printf("accpet file: %s \n",buf);
        memset(buf, 0, BUFFSIZE);
        strcpy(buf, "OK");
        rv = send(client_socket, buf, strlen(buf), 0);//回复OK 表名文件名已经收到
        if (rv <= 0)
        {
            printf("%s,%d:send error! \n",__FILE__,__LINE__);
            return  ERRORCODE;
        }
        else
        {
            printf("receive filename success, send back success!\n");
        }

        printf("start receive file......\n");
        while (1) // 接收文件
        {
            printf("循环接收文件。。\n");
            memset(buf, 0, BUFFSIZE);
            rv = recv(client_socket, buf, BUFFSIZE, 0);//接收文件内容
            if (rv <= 0)
            {
                printf("read over!!\n");
                if (rv < 0)
                {
                    printf("%s,%d:recv error!\n",__FILE__,__LINE__);
                    return  ERRORCODE;
                }

                break;
            }
            rv = fwrite(buf, 1, rv, fp);//写入文件
            printf("%s  ",buf);
            if (rv <= 0)
            {
                printf("%s,%d:file write error!",__FILE__,__LINE__);
                return ERRORCODE;
            }
            else
                printf("file write\n");
        }
#ifdef WIN
        closesocket(client_socket);
        printf("close client_socket.\n");
        if (fp)
        {
            fclose(fp);
        }

#else
        close(client_socket);
#endif // WIN
	}
#ifdef WIN
	closesocket(listen_socket);
	if (WSACleanup() == SOCKET_ERROR)
	{
		printf("WSACleanup failed with error %d\n", WSAGetLastError());
		return ERRORCODE;
	}
#else
	close(listen_socket);
#endif
	if (fp)
	{
		fclose(fp);
	}
	return 0;
}
