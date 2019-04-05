#ifndef PUB_H_INCLUDED
#define PUB_H_INCLUDED
//客户端发送函数入口
int send_work(const char *ip_str, int i_port, const char *filename);
//服务端发接收函数入口
int recv_work(int i_port);



#endif // PUB_H_INCLUDED
