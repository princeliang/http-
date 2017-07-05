/*
 首先是需要声明的头文件，我们需要重点记住的也就是sys/socket.h
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>


/*
 现在我们开始写main函数，期间会夹杂着
 声明一些我们需要用到的函数，这些函数
 不需要一次性写完，可以根据服务器的流程
 一个一个来，有什么都可以补充！
 */

//这是我们用到的第一个函数
//这个函数将套接字的建立，
//绑定和监听装在一起了。
int startup(u_short *port);




int main(void)

{
   //分别创建s_sock，c_sock用来接收服务器和客户端的套接字
   int s_sock,c_sock;
  

   u_short port=8000;//端口号，默认8000,可以自己修改


  //顶义sockaddr来绑定客户端的sockt
   struct sockaddr_in servername;

//我们调用的第一个自定义函数，startup是用来建立套接字，绑定，监听
   s_sock = startup(&port);

//昨天调试到49行

   int n;//用来记录epoll_wait返有多少个客户端就绪
   int i,j; 
   int num;//用来记录recv从客户端读取了多少字符

   //这个是epoll的根节点
   int efd;

   int sockfd;//用来存储取出的eopll中数组ep的socket

   int connfd;//用来接收accept返回的套接字（用来保存客户端的信息）


  //携带的event，类型是epoll_event，ep是数组，tep用来就收临时的客户端
   struct epoll_event tep,ep[1024];


  //用来接收客户端套接字的大小
   socklen_t clientname_len;

   
   

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




int startup(u_short *port)
{

int s_sock=0;
struct sockaddr_in server;

//调用了socket，创建server的套接字
s_sock=socket(AF_INET,SOCK_STREAM,0);

//在绑定套接字之前，需要对套接字地址
//进行一些初始化操作
memset(&server,0,sizeof(server));

//现在对server的套接字地址开始赋值
//里面的大部分参数很好记忆。不懂得
//可以去查阅sockaddr_in结构体的组成
server.sin_family=AF_INET;

//htons将无符号短整型从主机字节序
//转变成网络字节序。
server.sin_port=htons(*port);
server.sin_addr.s_addr=htonl(INADDR_ANY);//表示任何主机都可以和它链接

bind(s_sock,&server,sizeof(server));//绑定

listen(s_sock,1024);//监听

return s_sock;


}












