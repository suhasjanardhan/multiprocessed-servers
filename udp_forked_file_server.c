#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_SEND_BUF 1000
#define MAX_RECV_BUF 1000
#define MAX_DATA 1000


extern int errno;

int errexit(const char *format,...);

int connectUDP(const char *service,int portnum);
int connectsock(const char *service,int portnum,const char *transport);
void handler(int);

/*------------------------------------------------------------------------------------
 * connectsock-Allocate and connect socket for UDP
 *------------------------------------------------------------------------------------
*/

int connectsock(const char *service,int portnum,const char *transport)
{
/*
Arguments:
*service   - service associated with desired port
*transport - name of the transport protocol to use
*/
struct sockaddr_in server;                                                //an internet endpoint address

int server_socket,type,b,l,accept_socket,num;                             //two socket descriptors for listening and accepting 

memset(&server,0,sizeof(server));

server.sin_addr.s_addr=htons(INADDR_ANY);                                 //INADDR_ANY to match any IP address
server.sin_family=AF_INET;                                                //family name
server.sin_port=htons(portnum);                                              //port number

 
/*
 * to determine the type of socket
 */

if(strcmp(transport,"udp")==0)
{
type=SOCK_DGRAM;
}
else
{
type=SOCK_STREAM;
}


server_socket=socket(AF_INET,type,0);                                    //allocate a socket

if(server_socket<0)
{
printf("Socket can't be created\n");
exit(0);
}

/* to set the socket options- to reuse the given port multiple times */
num=1;

if(setsockopt(server_socket,SOL_SOCKET,SO_REUSEPORT,(const char*)&num,sizeof(num))<0)
{
printf("setsockopt(SO_REUSEPORT) failed\n");
exit(0);
}


/* bind the socket to known port */
b=bind(server_socket,(struct sockaddr*)&server,sizeof(server));

if(b<0)
{
printf("Error in binding\n");
exit(0);
}


return server_socket;

}



/*------------------------------------------------------------------------
 * connectUDP-connect to a specified UDP service on specified host
 -------------------------------------------------------------------------*/
int connectUDP(const char *service,int portnum)
{
/*
 Arguments:
 *service-service associated with desired port
 */
 return connectsock(service,portnum,"udp");
}


void handler(int sig)
{
 int status;
 while(wait3(&status,WNOHANG,(struct rusage *)0)>=0);
}


int errexit(const char* format,...)
{
va_list args;

va_start(args,format);
vfprintf(stderr,format,args);
va_end(args);
exit(1);
}


/*
 main - connectionless multiprocess server 
 */

int main(char argc,char *argv[])
{

char *service="echo";
	
        int portnum=atoi(argv[1]);

	int msock;
	char data[1000];
         char send_buf[MAX_SEND_BUF];
          int file; 
	/* call connectTCP to create a socket, bind it and place it in passive mode
	   once the call returns call accept on listening socket to accept the incoming requests
	 */

	msock=connectUDP(service,portnum);
        printf("Listening to client\n");

	(void) signal(SIGCHLD,handler);
          
         

	
   while(1)
         {
            struct sockaddr_in fsin;
	    
            int alen=sizeof(fsin);
	    

            int data_len;
		
	    data_len = recvfrom(msock,data,MAX_DATA,0,(struct sockaddr *)&fsin,&alen);
            
            
			
			
		if(data_len)
		{
		  printf("connected to multiforked connectionless server\n");
	          printf("File name recieved: %s\n", data);

		int pid;
		int x;
            		
	   	int file; 	
		
		switch (fork()) 
		{
			case 0: /* child */
			
			           
			if((file = open(data,O_RDONLY))<0)
			{       
				
				printf("File not found\n");
				printf("Client disconnected\n");
			}
			else
			{	
				
				printf("File opened successfully\n");
					
						
		

				
				
				ssize_t read_bytes;
 				ssize_t sent_bytes;
				 
				char send_buf[MAX_SEND_BUF]; 
				
	
				
				while((read_bytes = read(file, send_buf, MAX_RECV_BUF))>0)
				{
				 
				  printf("%s",send_buf);
				 
									 
				
					if( (sent_bytes = sendto(msock,send_buf,read_bytes,0,(struct sockaddr *)&fsin,sizeof(fsin)) < read_bytes))
					{
					  printf("send error\n");
					  return -1;
					}
				}
				close(file);
			 printf("\nclient disconnected\n");
			}
		 
			
			default: /* parent */
			
			break;
			case -1:
			printf("error in forking\n");
		}
				
        }    
		
	
    }
	    
   
 return 0;
 
}
    











