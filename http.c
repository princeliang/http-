#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<errno.h>

#define MAXLINE 80

int main(void)

{
   //分别创建s_sock，c_sock用来接收服务器和客户端的套接字
   int s_sock,c_sock;
 
   int n;//用来记录epoll_wait返有多少个客户端就绪
   int i,j; 
   char buf[1024];//读取数据的内存地址
   char str[16];//存储输出返回的IP地址
   int num;//用来记录recv从客户端读取了多少字符

   //这个是epoll的根节点
   int efd;

   int sockfd;//用来存储取出的eopll中数组ep的socket

   int connfd;//用来接收accept返回的套接字（用来保存客户端的信息）

   //自定义了8000端口用来做服务器的监听端口
   u_short port=8000;

  //携带的event，类型是epoll_event，ep是数组，tep用来就收临时的客户端
   struct epoll_event tep,ep[1024];


  //用来接收客户端套接字的大小
   socklen_t clientname_len;

  //分别定义sockaddr来绑定客户端和服务器的sockt
   struct sockaddr_in clientname;
   struct sockaddr_in servername;
   
  //初始化两个sockaddr
   bzero(&clientname,sizeof(clientname));
   bzero(&servername,sizeof(servername));
   
   clientname_len=sizeof(clientname);

   //创建了服务器的套接字
   s_sock=socket(AF_INET,SOCK_STREAM,0);
  
   //用来绑定sever套接字和端口
   servername.sin_family=AF_INET;
   servername.sin_port=htons(port);
   servername.sin_addr.s_addr=htons(INADDR_ANY);
   
   //绑定套接字和端口
   bind(s_sock,(struct sockaddr *) &servername,sizeof(servername));
   
   //监听端口
   listen(s_sock,128);

   //创建epoll的根节点   
   efd=epoll_create(1024);
  
  //添加服务器的socket到epoll的根节点中
   tep.events=EPOLLIN;
   tep.data.fd=s_sock;
   int res=epoll_ctl(efd,EPOLL_CTL_ADD,s_sock,&tep);
   
   while(1)
  {   
    //阻塞监听
     n=epoll_wait(efd,ep,1024,-1);
    
    for(i=0;i<n;i++)
       {
	//如果是listen监听的fd，代表有新的客户端要来连接了
	if(ep[i].data.fd==s_sock)	
           {
		 connfd=accept(s_sock,(struct sockaddr *)&clientname,&clientname_len);
		printf("receiced from %s at PORT %d\n",inet_ntop(AF_INET,&clientname.sin_addr,str,16),port);
		
		tep.events=EPOLLIN;
		tep.data.fd=connfd;
		res=epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&tep);
	   }   
	//如果不是listen监听的fd，那么就是已经连接的客户端的数据
  	else
           {
		memset(buf,'\0',sizeof(buf));
		sockfd=ep[i].data.fd;
		num=recv(sockfd,buf,sizeof(buf),0);
		if(num==0)
			{
			   res=epoll_ctl(efd,EPOLL_CTL_DEL,sockfd,NULL);
			   close(sockfd);
			   printf("client[%d] closed connection\n",i);
			}						
		else
			{
			  for(j=0;j<num;j++)
				buf[j]=toupper(buf[j]);
			  send(sockfd,buf,strlen(buf),0);
			}
           }

       }
    }
     return 0;
}


