#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  srand(time(NULL));
  pid_t pid,waitpid;
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  int i,j;
  int selected[10];
  char buffer[256];

  if (argc < 3) 
  {
    fprintf(stderr,"Error %s\n", argv[0]);
    exit(0);
  }

  portno = atoi(argv[2]); 
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 

  if (sockfd < 0) 
    error("Error in opening socket");

  server = gethostbyname(argv[1]);   

  if (server == NULL) 
  {
    fprintf(stderr,"Error, there is no such a host\n");
    exit(0);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr)); 
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  serv_addr.sin_port = htons(portno);     

  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    error("Error connecting");

		printf("\n-----------New Customer----------\n");
  for (j=0; j<10; j++)
    {
      printf("Order Number:%d \n",j+1); 
			selected[j] = (rand() % 20);
      sleep(1);
			printf("Product Selected By Customer: %d\n",selected[j]);
				
			n=write(sockfd,&selected[j],sizeof(int));

      if (n < 0) 
        error("ERROR writing to socket");

      bzero(buffer,256);
      n = read(sockfd,buffer,255);

      if (n < 0) 
        error("ERROR reading from socket");

      printf("%s\n",buffer);
    } 		  
  close(sockfd); 
}