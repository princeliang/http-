#include <stdio.h>

int main(void)

{
   //�ֱ𴴽�s_sock��c_sock�������շ������Ϳͻ��˵��׽���
   int s_sock,c_sock;
 
   //�����epoll�ĸ��ڵ�
   int efd;

   //�Զ�����8000�˿��������������ļ����˿�
   u_short port=8000;

  //Я����event��������epoll_event��ep�����飬tep����������ʱ�Ŀͻ���
   struct epoll_event tep,ep[1024];


  //�������տͻ����׽��ֵĴ�С
   socklen_t clientname_len;
   clientname_len=sizeof(cilentname)

  //�ֱ���sockaddr���󶨿ͻ��˺ͷ�������sockt
   struct sockaddr_in clientname;
   struct sockaddr_in servername;

   clientname_len=sizeof(cilentname)

   //�����˷��������׽���
   s_sock=socket(AF_INET,SOCK_STREAM,0);
  
   //������sever�׽��ֺͶ˿�
   severname.sin_family=AF_INET;
   severname.sin_port=htons(*port);
   severname.sin_addr=htons(INADDR_ANY);
   
   //���׽��ֺͶ˿�
   bind(s_sock,(struct sockaddr *) &servername,sizeof(servername));
   
   //�����˿�
   listen(s_sock,128);
      
   efd=epoll_creat(1024);
  
   tep.events=EPOLLIN;
   tep.data.fd=s_sock;

















}



