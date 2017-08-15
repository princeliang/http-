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

//这个函数将套接字的建立，绑定和监听装在一起了。
int startup(u_short *port);

//这个是创建epoll的函数
int createepoll();

//这个是处理请求函数
void accept_request(int sockfd);

//这个是用来一行一行提取http报文中的信息的
int get_line(int sock,char*buf,int size);




int main(void)

{
   //分别创建s_sock，c_sock用来接收服务器和客户端的套接字
   int s_sock,c_sock;
  

   u_short port=8000;//端口号，默认8000,可以自己修改


  //顶义sockaddr来绑定客户端的sockt
   struct sockaddr_in clientname;
   socklen_t clientname_len;//用来存储clientname有多少个字节

//我们调用的第一个自定义函数，startup是用来建立套接字，绑定，监听
   s_sock = startup(&port);


//接下来我们开始进入重点：epoll
//createpoll用来创建epoll的根节点
int efd;//用来接收根节点
//ep是用户空间提供的一个数组，用来接收返回的event
epoll_event ep[1024];
epoll_event tep;//临时存储接收的客户端
efd=createepoll();
int sockfd;//这个是用来取出已经连接并请求数据的客户端的fd
//开始阻塞知道客户建立链接
   while(1)
  {   
 //永久阻塞来监听
  int n= epoll_wait(efd,ep,1024,-1);//epoll成功返回准备就绪文件描述符的个数

   for(i=0;i<n;i++)
       {
	//如果是listen监听的s_sock的fd，代表有新的客户端要来连接了,将新的客户端加入epoll
	if(ep[i].data.fd==s_sock)	
           {
		 c_sock=accept(s_sock,&clientname,&clientname_len);
		tep.events=EPOLLIN;
		tep.data.fd=c_sock;
		epoll_ctl(efd,EPOLL_CTL_ADD,c_sock,&tep);
	   }   
	//如果不是listen监听的fd，那么就是已经连接的客户端的数据
	else
	{
		//先把fd给sockfd，然后交给处理请求函数去处理
		sockfd=ep[i].data.fd;
		//这是我们用的第三个函数，处理请求函数，这个函数很大一部分体现了服务器处理请求流程，是一个重点
		accept_request(sockfd);
		
	}

}


    
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


   int n;//用来记录epoll_wait返有多少个客户端就绪
   int i,j; 
   int num;//用来记录recv从客户端读取了多少字符

   //这个是epoll的根节点

   int sockfd;//用来存储取出的eopll中数组ep的socket

   int connfd;//用来接收accept返回的套接字（用来保存客户端的信息）


  //携带的event，类型是epoll_event，ep是数组，tep用来就收临时的客户端
   struct epoll_event tep,ep[1024];


  //用来接收客户端套接字的大小
   socklen_t clientname_len;

   
   

   efd=epoll_create(1024);
  
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


int createepoll()
{
int edf=epoll_create(1024);

  //添加服务器socket到epoll的根节点中
  //至于epoll_event的结构可以自己去查，
  //其中data这个子类型我们主要用的就是fd
 epoll_event tep; 
 tep.events=EPOLLIN;
 tep.data.fd=s_sock;
 int res=epoll_ctl(efd,EPOLL_CTL_ADD,s_sock,&tep);//这个函数成功返回0，失败返回-1;
//需要返回epoll的根节点,这样在主函数可以用epoll_wait监听了！
return edf;
}


void accept_request(int sockfd)
{
	//这个函数比较难，我们慢慢写
	
	char buf[1024];//用来接收套接字中的信息的
	int numchars;//这个用来接收git_line的数据
	char method[255];//存储http报文中的访问方法
	char url[255];//存储http报文中的统一资源定位地址
	char path[512];//存储请问问价的路径的
	size_t i, j;
	struct stat st;
	int cgi = 0;//判断是否调用cgi程序的   
	char *query_string = NULL;//这个是用来指向提交的数据的指针
	
	//这里用到了get_line函数，用来分解http报文
	numchars = get_line(sock, buf, sizeof(buf));

        //开始解析我们读取到的数据
	i = 0; j = 0;
	//根据http报文的特性，找空格，隔离出访问方法，用ISspace(char x)函数直接判断是否为空格
	while (!ISspace(buf[i]) && (i < sizeof(method) - 1))
	{
		method[i] = buf[i];
		i++;
	}
	j=i;
	method[i] = '\0';//method里面已经存的是方法


	//开始看是那种方法，因为我们只实现了GET,POST,所以如果不是这两种方法的话，我们返回提示信息
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
	{
		unimplemented(client);
		return;
	}

	//如果是post方法，我们需要调用cgi程序
	if (strcasecmp(method, "POST") == 0)
		cgi = 1;
	
	//开始解析url地址信息
	i = 0;
	while (ISspace(buf[j]) && (j < numchars))
		j++;
	while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars))
	{
		url[i] = buf[j];
		i++; j++;
	}
	url[i] = '\0';

	//如果是GET方法，我们需要看他有没有提交数据，GET的数据都在url后用“？”隔开
	//如果GET方法又提交数据，那么我们需要调用cgi
	if (strcasecmp(method, "GET") == 0)
	{
		query_string = url;
		while ((*query_string != '?') && (*query_string != '\0'))
			query_string++;
		if (*query_string == '?')
		{
			cgi = 1;
			*query_string = '\0';
			query_string++;
		}
	}

	//这个是我们自己服务器文件的存放目录，我们要拼接目录生成本地路径，把url给path指针
	sprintf(path, "htdocs%s", url);
	
	//判断url是不是 / 结尾，是的话我们就直接返回目录文件 index.html
	if (path[strlen(path) - 1] == '/')
		strcat(path, "index.html");
	//判断请求资源的状态
	if (stat(path, &st) == -1) {
	    	//如果请求资源无法访问那么读取buf中剩下的报文并丢弃
		while ((numchars > 0) && strcmp("\n", buf))  
			numchars = get_line(client, buf, sizeof(buf));
		not_found(client);
	}
	else
	{
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path, "/index.html");
		if ((st.st_mode & S_IXUSR) ||
				(st.st_mode & S_IXGRP) ||
				(st.st_mode & S_IXOTH)    )
			cgi = 1;
		if (!cgi)
			serve_file(client, path);
		else
			execute_cgi(client, path, method, query_string);
	}

	close(client);

}




int get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';//因为很多http报文结束用的是\r\n,我们需要把它全部转化成\n结尾，标准化buf数组
	int n;
//下面是一个简单的算法
	while ((i < size - 1) && (c != '\n'))//如果没读完或者c！=“\n“，就进入循环
	{
		n = recv(sock, &c, 1, 0);
//recv最后一个参数flag为0，recv读取tcp buffer中的数据到buf中，并且移除已经读取的数据，
//我们的buf是字符c，用recv一个一个的读取，成功copy数据后返回copy的字节数，失败返回-1
	
		if (n > 0)//开始判断copy回来的字节
		{
			if (c == '\r')//判断是不是\r结尾，是的话判断下一个是不是\n结尾
			{
				n = recv(sock, &c, 1, MSG_PEEK);//这里的最后一个flag参数是MSG_PEEK，这个读取的数据不会从tcp中移除
                                                                //用来判断下一个字符是什么
		
				if ((n > 0) && (c == '\n')) //如果是\n，那么我们可以真正读取到c中
					recv(sock, &c, 1, 0);
				else
					c = '\n';//如果不是\n那么我们直接让c=‘\n’,这样不管是\r结尾，还是\r\n结尾，都转成\n结尾			}
			buf[i] = c;//如果不是\r的字符，那么就是有用字符，需要写到buf中去
			i++;
		}
		else
			c = '\n';//读取失败，代表读完了，直接用\n结尾
	}
	buf[i] = '\0';//手动给字符数组加\0结尾

	return(i);//返回的是i，字符数组的最大下标。
}





