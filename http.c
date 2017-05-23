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
   //�ֱ𴴽�s_sock��c_sock�������շ������Ϳͻ��˵��׽���
   int s_sock,c_sock;
 
   int n;//������¼epoll_wait���ж��ٸ��ͻ��˾���
   int i,j; 
   char buf[1024];//��ȡ���ݵ��ڴ��ַ
   char str[16];//�洢������ص�IP��ַ
   int num;//������¼recv�ӿͻ��˶�ȡ�˶����ַ�

   //�����epoll�ĸ��ڵ�
   int efd;

   int sockfd;//�����洢ȡ����eopll������ep��socket

   int connfd;//��������accept���ص��׽��֣���������ͻ��˵���Ϣ��

   //�Զ�����8000�˿��������������ļ����˿�
   u_short port=8000;

  //Я����event��������epoll_event��ep�����飬tep����������ʱ�Ŀͻ���
   struct epoll_event tep,ep[1024];


  //�������տͻ����׽��ֵĴ�С
   socklen_t clientname_len;

  //�ֱ���sockaddr���󶨿ͻ��˺ͷ�������sockt
   struct sockaddr_in clientname;
   struct sockaddr_in servername;
   
  //��ʼ������sockaddr
   bzero(&clientname,sizeof(clientname));
   bzero(&servername,sizeof(servername));
   
   clientname_len=sizeof(clientname);

   //�����˷��������׽���
   s_sock=socket(AF_INET,SOCK_STREAM,0);
  
   //������sever�׽��ֺͶ˿�
   servername.sin_family=AF_INET;
   servername.sin_port=htons(port);
   servername.sin_addr.s_addr=htons(INADDR_ANY);
   
   //���׽��ֺͶ˿�
   bind(s_sock,(struct sockaddr *) &servername,sizeof(servername));
   
   //�����˿�
   listen(s_sock,128);

   //����epoll�ĸ��ڵ�   
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


