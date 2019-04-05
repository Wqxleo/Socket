//建立socket
//确定服务器scokaddr_in结构体
//    点分十进制IP转换
//使用connect连接
//打开文件
//准备缓冲区
//缓冲区初始化置空
//将文件内容读入缓冲区
//使用send将缓冲区内容发送到服务器
//文件内容发送完成
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

#define BUFFER_SIZE 1024



int main()
{
    int sockcd;
    struct sockaddr_in server;
    char filepath[100];//file to translate

    FILE *fp;
    int lenpath; //filepath length
    char *buffer;//file buffer
    int fileTrans;
    buffer = (char *)malloc(sizeof(char)*BUFFER_SIZE);
    bzero(buffer,BUFFER_SIZE);
    //memset(buffer,0,sizeof(buffer));

    if((sockcd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        printf("socket build error!\n");
    }
    memset(&server,0,sizeof(server));
    server.sin_family= AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);	/*本地地址*/
    server.sin_port = htons(6666);

    if(connect(sockcd,(struct sockaddr*)&server,sizeof(server))<0)
    {
        printf("connect error!\n");
    }//connect with server

    printf("file path:\n");
    scanf("%s",filepath);//get filepath

    fp = fopen(filepath,"r");//opne file
    if(fp==NULL)
    {
        printf("filepath not found!\n");
        return 0;

    }
    printf("filepath : %s\n",filepath);
    lenpath = send(sockcd,filepath,strlen(filepath),0);// put file path to sever
    if(lenpath<0)
    {
        printf("filepath send error!\n");
    }
    else
    {
        printf("filepath send success!\n");
    }
    sleep(3);
    while((fileTrans = fread(buffer,sizeof(char),BUFFER_SIZE,fp))>0)
    {
        printf("fileTrans =%d\n",fileTrans);
        if(send(sockcd,buffer,fileTrans,0)<0)
        {
            printf("send failed!\n");
            break;
        }
        bzero(buffer,BUFFER_SIZE);
        //memset(buffer,0,sizeof(buffer));
    }
    fclose(fp);

    close(sockcd);



    return 0;
}
