//����socket
//ȷ��������scokaddr_in�ṹ��
//    ���ʮ����IPת��
//ʹ��connect����
//���ļ�
//׼��������
//��������ʼ���ÿ�
//���ļ����ݶ��뻺����
//ʹ��send�����������ݷ��͵�������
//�ļ����ݷ������
//�ر��ļ�
//�ر�socket

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
	server.sin_addr.s_addr = htonl(INADDR_ANY);	/*���ص�ַ*/
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