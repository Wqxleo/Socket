#include<stdio.h>
#include<stdlib.h>
#include"pub.h"

int main(int argc, char *argv[])
{
	int i_port = 0;
	if (argc <= 1)
	{
		printf("Usage:port \n");
		return -1;
	}

	i_port = atoi(argv[1]); //
	if (i_port == 0)
	{
		printf("port error!");
		return -1;
	}

	if (recv_work(i_port) ==0)
	{
		printf("recv success! \n");
	} else
	{
		printf("recv failure! \n");
	}

	return 0;
}
