/*
 ��������Ҫ������ͷ�ļ���������Ҫ�ص��ס��Ҳ����sys/socket.h
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
 �������ǿ�ʼдmain�������ڼ�������
 ����һЩ������Ҫ�õ��ĺ�������Щ����
 ����Ҫһ����д�꣬���Ը��ݷ�����������
 һ��һ��������ʲô�����Բ��䣡
 */

//���������õ��ĵ�һ������
//����������׽��ֵĽ�����
//�󶨺ͼ���װ��һ���ˡ�
int startup(u_short *port);




int main(void)

{
   //�ֱ𴴽�s_sock��c_sock�������շ������Ϳͻ��˵��׽���
   int s_sock,c_sock;
  

   u_short port=8000;//�˿ںţ�Ĭ��8000,�����Լ��޸�


  //����sockaddr���󶨿ͻ��˵�sockt
   struct sockaddr_in servername;

//���ǵ��õĵ�һ���Զ��庯����startup�����������׽��֣��󶨣�����
   s_sock = startup(&port);

//������Ե�49��

   int n;//������¼epoll_wait���ж��ٸ��ͻ��˾���
   int i,j; 
   int num;//������¼recv�ӿͻ��˶�ȡ�˶����ַ�

   //�����epoll�ĸ��ڵ�
   int efd;

   int sockfd;//�����洢ȡ����eopll������ep��socket

   int connfd;//��������accept���ص��׽��֣���������ͻ��˵���Ϣ��


  //Я����event��������epoll_event��ep�����飬tep����������ʱ�Ŀͻ���
   struct epoll_event tep,ep[1024];


  //�������տͻ����׽��ֵĴ�С
   socklen_t clientname_len;

   
   

   efd=epoll_create(1024);
  
  //��ӷ�������socket��epoll�ĸ��ڵ���
   tep.events=EPOLLIN;
   tep.data.fd=s_sock;
   int res=epoll_ctl(efd,EPOLL_CTL_ADD,s_sock,&tep);
   
   while(1)
  {   
    //��������
     n=epoll_wait(efd,ep,1024,-1);
    
    for(i=0;i<n;i++)
       {
	//�����listen������fd���������µĿͻ���Ҫ��������
	if(ep[i].data.fd==s_sock)	
           {
		 connfd=accept(s_sock,(struct sockaddr *)&clientname,&clientname_len);
		printf("receiced from %s at PORT %d\n",inet_ntop(AF_INET,&clientname.sin_addr,str,16),port);
		
		tep.events=EPOLLIN;
		tep.data.fd=connfd;
		res=epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&tep);
	   }   
	//�������listen������fd����ô�����Ѿ����ӵĿͻ��˵�����
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

//������socket������server���׽���
s_sock=socket(AF_INET,SOCK_STREAM,0);

//�ڰ��׽���֮ǰ����Ҫ���׽��ֵ�ַ
//����һЩ��ʼ������
memset(&server,0,sizeof(server));

//���ڶ�server���׽��ֵ�ַ��ʼ��ֵ
//����Ĵ󲿷ֲ����ܺü��䡣������
//����ȥ����sockaddr_in�ṹ������
server.sin_family=AF_INET;

//htons���޷��Ŷ����ʹ������ֽ���
//ת��������ֽ���
server.sin_port=htons(*port);
server.sin_addr.s_addr=htonl(INADDR_ANY);//��ʾ�κ����������Ժ�������

bind(s_sock,&server,sizeof(server));//��

listen(s_sock,1024);//����

return s_sock;


}












