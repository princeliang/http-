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

//����������׽��ֵĽ������󶨺ͼ���װ��һ���ˡ�
int startup(u_short *port);

//����Ǵ���epoll�ĺ���
int createepoll();

//����Ǵ���������
void accept_request(int sockfd);

//���������һ��һ����ȡhttp�����е���Ϣ��
int get_line(int sock,char*buf,int size);




int main(void)

{
   //�ֱ𴴽�s_sock��c_sock�������շ������Ϳͻ��˵��׽���
   int s_sock,c_sock;
  

   u_short port=8000;//�˿ںţ�Ĭ��8000,�����Լ��޸�


  //����sockaddr���󶨿ͻ��˵�sockt
   struct sockaddr_in clientname;
   socklen_t clientname_len;//�����洢clientname�ж��ٸ��ֽ�

//���ǵ��õĵ�һ���Զ��庯����startup�����������׽��֣��󶨣�����
   s_sock = startup(&port);


//���������ǿ�ʼ�����ص㣺epoll
//createpoll��������epoll�ĸ��ڵ�
int efd;//�������ո��ڵ�
//ep���û��ռ��ṩ��һ�����飬�������շ��ص�event
epoll_event ep[1024];
epoll_event tep;//��ʱ�洢���յĿͻ���
efd=createepoll();
int sockfd;//���������ȡ���Ѿ����Ӳ��������ݵĿͻ��˵�fd
//��ʼ����֪���ͻ���������
   while(1)
  {   
 //��������������
  int n= epoll_wait(efd,ep,1024,-1);//epoll�ɹ�����׼�������ļ��������ĸ���

   for(i=0;i<n;i++)
       {
	//�����listen������s_sock��fd���������µĿͻ���Ҫ��������,���µĿͻ��˼���epoll
	if(ep[i].data.fd==s_sock)	
           {
		 c_sock=accept(s_sock,&clientname,&clientname_len);
		tep.events=EPOLLIN;
		tep.data.fd=c_sock;
		epoll_ctl(efd,EPOLL_CTL_ADD,c_sock,&tep);
	   }   
	//�������listen������fd����ô�����Ѿ����ӵĿͻ��˵�����
	else
	{
		//�Ȱ�fd��sockfd��Ȼ�󽻸�����������ȥ����
		sockfd=ep[i].data.fd;
		//���������õĵ�������������������������������ܴ�һ���������˷����������������̣���һ���ص�
		accept_request(sockfd);
		
	}

}


    
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


   int n;//������¼epoll_wait���ж��ٸ��ͻ��˾���
   int i,j; 
   int num;//������¼recv�ӿͻ��˶�ȡ�˶����ַ�

   //�����epoll�ĸ��ڵ�

   int sockfd;//�����洢ȡ����eopll������ep��socket

   int connfd;//��������accept���ص��׽��֣���������ͻ��˵���Ϣ��


  //Я����event��������epoll_event��ep�����飬tep����������ʱ�Ŀͻ���
   struct epoll_event tep,ep[1024];


  //�������տͻ����׽��ֵĴ�С
   socklen_t clientname_len;

   
   

   efd=epoll_create(1024);
  
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


int createepoll()
{
int edf=epoll_create(1024);

  //��ӷ�����socket��epoll�ĸ��ڵ���
  //����epoll_event�Ľṹ�����Լ�ȥ�飬
  //����data���������������Ҫ�õľ���fd
 epoll_event tep; 
 tep.events=EPOLLIN;
 tep.data.fd=s_sock;
 int res=epoll_ctl(efd,EPOLL_CTL_ADD,s_sock,&tep);//��������ɹ�����0��ʧ�ܷ���-1;
//��Ҫ����epoll�ĸ��ڵ�,������������������epoll_wait�����ˣ�
return edf;
}


void accept_request(int sockfd)
{
	//��������Ƚ��ѣ���������д
	
	char buf[1024];//���������׽����е���Ϣ��
	int numchars;//�����������git_line������
	char method[255];//�洢http�����еķ��ʷ���
	char url[255];//�洢http�����е�ͳһ��Դ��λ��ַ
	char path[512];//�洢�����ʼ۵�·����
	size_t i, j;
	struct stat st;
	int cgi = 0;//�ж��Ƿ����cgi�����   
	char *query_string = NULL;//���������ָ���ύ�����ݵ�ָ��
	
	//�����õ���get_line�����������ֽ�http����
	numchars = get_line(sock, buf, sizeof(buf));

        //��ʼ�������Ƕ�ȡ��������
	i = 0; j = 0;
	//����http���ĵ����ԣ��ҿո񣬸�������ʷ�������ISspace(char x)����ֱ���ж��Ƿ�Ϊ�ո�
	while (!ISspace(buf[i]) && (i < sizeof(method) - 1))
	{
		method[i] = buf[i];
		i++;
	}
	j=i;
	method[i] = '\0';//method�����Ѿ�����Ƿ���


	//��ʼ�������ַ�������Ϊ����ֻʵ����GET,POST,����������������ַ����Ļ������Ƿ�����ʾ��Ϣ
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
	{
		unimplemented(client);
		return;
	}

	//�����post������������Ҫ����cgi����
	if (strcasecmp(method, "POST") == 0)
		cgi = 1;
	
	//��ʼ����url��ַ��Ϣ
	i = 0;
	while (ISspace(buf[j]) && (j < numchars))
		j++;
	while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars))
	{
		url[i] = buf[j];
		i++; j++;
	}
	url[i] = '\0';

	//�����GET������������Ҫ������û���ύ���ݣ�GET�����ݶ���url���á���������
	//���GET�������ύ���ݣ���ô������Ҫ����cgi
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

	//����������Լ��������ļ��Ĵ��Ŀ¼������Ҫƴ��Ŀ¼���ɱ���·������url��pathָ��
	sprintf(path, "htdocs%s", url);
	
	//�ж�url�ǲ��� / ��β���ǵĻ����Ǿ�ֱ�ӷ���Ŀ¼�ļ� index.html
	if (path[strlen(path) - 1] == '/')
		strcat(path, "index.html");
	//�ж�������Դ��״̬
	if (stat(path, &st) == -1) {
	    	//���������Դ�޷�������ô��ȡbuf��ʣ�µı��Ĳ�����
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
	char c = '\0';//��Ϊ�ܶ�http���Ľ����õ���\r\n,������Ҫ����ȫ��ת����\n��β����׼��buf����
	int n;
//������һ���򵥵��㷨
	while ((i < size - 1) && (c != '\n'))//���û�������c��=��\n�����ͽ���ѭ��
	{
		n = recv(sock, &c, 1, 0);
//recv���һ������flagΪ0��recv��ȡtcp buffer�е����ݵ�buf�У������Ƴ��Ѿ���ȡ�����ݣ�
//���ǵ�buf���ַ�c����recvһ��һ���Ķ�ȡ���ɹ�copy���ݺ󷵻�copy���ֽ�����ʧ�ܷ���-1
	
		if (n > 0)//��ʼ�ж�copy�������ֽ�
		{
			if (c == '\r')//�ж��ǲ���\r��β���ǵĻ��ж���һ���ǲ���\n��β
			{
				n = recv(sock, &c, 1, MSG_PEEK);//��������һ��flag������MSG_PEEK�������ȡ�����ݲ����tcp���Ƴ�
                                                                //�����ж���һ���ַ���ʲô
		
				if ((n > 0) && (c == '\n')) //�����\n����ô���ǿ���������ȡ��c��
					recv(sock, &c, 1, 0);
				else
					c = '\n';//�������\n��ô����ֱ����c=��\n��,����������\r��β������\r\n��β����ת��\n��β			}
			buf[i] = c;//�������\r���ַ�����ô���������ַ�����Ҫд��buf��ȥ
			i++;
		}
		else
			c = '\n';//��ȡʧ�ܣ���������ˣ�ֱ����\n��β
	}
	buf[i] = '\0';//�ֶ����ַ������\0��β

	return(i);//���ص���i���ַ����������±ꡣ
}





