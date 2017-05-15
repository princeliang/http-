#include <stdio.h>

int main(void)

{
   //分别创建s_sock，c_sock用来接收服务器和客户端的套接字
   int s_sock,c_sock;
 
   //这个是epoll的根节点
   int efd;

   //自定义了8000端口用来做服务器的监听端口
   u_short port=8000;

  //携带的event，类型是epoll_event，ep是数组，tep用来就收临时的客户端
   struct epoll_event tep,ep[1024];


  //用来接收客户端套接字的大小
   socklen_t clientname_len;
   clientname_len=sizeof(cilentname)

  //分别定义sockaddr来绑定客户端和服务器的sockt
   struct sockaddr_in clientname;
   struct sockaddr_in servername;

   clientname_len=sizeof(cilentname)

   //创建了服务器的套接字
   s_sock=socket(AF_INET,SOCK_STREAM,0);
  
   //用来绑定sever套接字和端口
   severname.sin_family=AF_INET;
   severname.sin_port=htons(*port);
   severname.sin_addr=htons(INADDR_ANY);
   
   //绑定套接字和端口
   bind(s_sock,(struct sockaddr *) &servername,sizeof(servername));
   
   //监听端口
   listen(s_sock,128);
      
   efd=epoll_creat(1024);
  
   tep.events=EPOLLIN;
   tep.data.fd=s_sock;

















}



