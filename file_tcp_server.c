//建立socket
//确定服务器scokaddr_in结构体
//    点分十进制IP转换
//使用bind绑定套接字
//使用listen监听
//使用accept接受连接请求
//    accept返回新的套接字描述符
//使用recv接收传来的数据（文件路径）
//打开文件，这里需要文件名
//    从该字符串获取文件名
//使用recv接收文件内容
//    判断recv函数返回的状态
//将接收到的内容放入缓冲区
//将缓冲区内容写入文件
//关闭文件
//关闭socket

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>  //for malloc

#define MAXLINE 4096
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;  //服务器的Socket描述符，
    int temp,n;
    struct sockaddr_in server,client; //服务器地址结构和客户端地址结构
    char filename[100];  //文件名
    char filepath[100];   //文件路径

    char *buffer;//file buffer  文件缓冲区
    int fileTrans;

    buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
    bzero(buffer,BUFFER_SIZE);
    //memset(buffer,0,sizeof(buffer));
    int lenfilepath;
    FILE *fp;
    int writelength;

	//建立一个流式套接字 TCP连接
    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        printf("socket build error!\n");
    }
    else
    {
        printf("socket build success!\n");
    }

	//设置服务器地址
    memset(&server,0,sizeof(server));  //清零  等于 bzero(&server,sizeof(server))
    server.sin_family= AF_INET;  //协议族
    server.sin_addr.s_addr = htonl(INADDR_ANY); //本机地址
    server.sin_port = htons(6666);  //端口号


	//绑定地址结构到套接字描述符
    if((bind(sockfd,(struct sockaddr*)&server,sizeof(server)))==-1)
    {
        printf("bind error!\n");
    }
    else
    {
        printf("bind success!\n");
    }

	//设置监听
    if(listen(sockfd,10)==-1)
    {
        printf("listen error!\n");
    }
    else
    {
        printf("listen success!\n");
    }
    printf("waiting..\n");
    while(1)
    {
        if((temp = accept(sockfd,(struct sockaddr*)NULL,NULL))>0)
        {
            //printf("client IP:   %d\n",ntohl(client.sin_addr.s_addr));
            //printf("client PORT: %d\n",ntohs(client.sin_port));

            memset(filename,'\0',sizeof(filename));
            memset(filepath,'\0',sizeof(filepath));

            lenfilepath = recv(temp,filepath,100,0);
            printf("filepath :%s\n",filepath);
            if(lenfilepath<0)
            {
                printf("recv error!\n");
            }
            else
            {
                int i=0,k=0;
                for(i=strlen(filepath);i>=0;i--)
                {
                    if(filepath[i]!='/')
                    {
                        k++;
                    }
                    else
                        break;
                }
                strcpy(filename,filepath+(strlen(filepath)-k)+1);
            }
            printf("filename :%s\n",filename);
            fp = fopen(filename,"w");
            if(fp!=NULL)
            {
                while(fileTrans =recv(temp,buffer,BUFFER_SIZE,0))
                {
                    if(fileTrans<0)
                    {
                        printf("recv error!\n");
                        break;
                    }
                    writelength = fwrite(buffer,sizeof(char),fileTrans,fp);
                    if(writelength <fileTrans)
                    {
                        printf("write error!\n");
                        break;
                    }
                    bzero(buffer,BUFFER_SIZE);
                    //memset(buffer,0,sizeof(buffer));
                }
                printf("recv finished!\n");
                fclose(fp);
            }
            else
            {
                printf("filename is null!\n");

            }
            close(temp);


        }
        else
        {
            printf("accept error!\n");
            close(temp);
        }

    }

    close(sockfd);

    return 0;

}
