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
#include <arpa/inet.h>

#define BUFFER_SIZE 256
#define EURO 300.0f

struct product
{	
    int description;
	float price;
	int item_count;
 	int request_count;
	int sold_count;
	int customerNotSupportedCtr[5];
};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    struct product catalog[20];
    int socketfd, newsocketfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
	srand(time(NULL));
	time_t t;
	int retVal,status;
    char buffer[256];
	int selected[10],operation,client;
	int ordersCtr=0, orderSuccessfulCtr=0, orderUnsuccessfulCtr=0;
	float totalTurnover=0.0f; 
    socklen_t clilen;  
    int buf,pid;
	int order_result;
    int fd[2];
    int n,i,j,k,p;

    if (argc < 2) 
    {
        fprintf(stderr,"Error no port found.\n");
        exit(1);
    }

    printf("----------------CATALOG------------\n");
	printf("Product Description: \t Product Availability: \t Product Price: \t \n");
	for(i=0;i<20;i++)
	{
		catalog[i].item_count=2;
		catalog[i].description=i;
		catalog[i].price=((float)rand()/RAND_MAX)*(float)(101);   
		printf("%d\t\t\t %d\t\t\t %.2f \n",catalog[i].description,catalog[i].item_count,catalog[i].price);
		catalog[i].request_count=0;
		catalog[i].sold_count=0;
	}

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0) 
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(socketfd,5);

    clilen = sizeof(cli_addr);

    
    while (1)
    {
        retVal = pipe(fd);
		
		if(retVal==-1)
        {
        	perror("PIPE #1 ERROR.");
       		exit(1);
        }

        printf("\n");
        newsocketfd = accept(socketfd,(struct sockaddr *) &cli_addr, &clilen);

        if (newsocketfd < 0) 
            error("ERROR on accept");
        pid=fork();

        if (pid < 0)
            error("ERROR in new process creation");
        if (pid == 0) 
        {
            for (p=0;p<10;p++)
		    {
                
			    n= read(newsocketfd,&buf,sizeof(int));  

                if (n < 0) error("ERROR writing to socket");
                catalog[buf].request_count++;

                if (catalog[buf].item_count>0)
				{
                    catalog[i].sold_count++;
				    n = write(newsocketfd,"Product Available!",19);
                    if (n < 0) error("ERROR writing to socket\n");
                    operation=catalog[buf].item_count-1;
					catalog[buf].customerNotSupportedCtr[p]=0;


                    close(fd[0]);
                    write(fd[1],&buf,sizeof(int));
                    catalog[buf].item_count=catalog[buf].item_count-1;
                    catalog[buf].sold_count++;
				}
				else
				{
				    n = write(newsocketfd,"Product Unavailable!",21);
                    if (n < 0) error("ERROR writing to socket");
                    operation=0;
                    catalog[buf].customerNotSupportedCtr[p]++;
                    orderUnsuccessfulCtr++;
                    close(fd[0]);
                    write(fd[1],&operation,sizeof(int));
                    
        		}  

                catalog[buf].request_count++;
            }   
            close(newsocketfd);
            close(socketfd);
            exit (0);
        }
        else
        {
            client++;
            for (int m = 0; m < 10; m++)
            {
                int newop;
                close(fd[1]);
                read(fd[0],&newop,sizeof(int));
                catalog[newop].item_count=catalog[newop].item_count-1;
                
                orderSuccessfulCtr++;
                totalTurnover+=catalog[buf].price;
                catalog[buf].sold_count++;
            }
            close(newsocketfd);
            wait(NULL);
        }
        
        ordersCtr=orderSuccessfulCtr + orderUnsuccessfulCtr;

        if(client==5)
        {
            printf("-------------------INFORMATION:--------------------\n");
            for(i=0;i<20;i++)
	        {
                catalog[i].sold_count++;
                printf("\nProduct Description:%d\n",catalog[i].description);
		        printf("Requested %d Times.\n",catalog[i].request_count);
		        printf("Sold Products %d\n",catalog[i].sold_count);

                if(catalog[i].sold_count>=2)
		            printf("\nSold Out\n");
		        for(j=0; j<5; j++)
		        {
			        if(catalog[i].customerNotSupportedCtr[j]>0 && catalog[i].customerNotSupportedCtr[j]<5)
			        {
				        //printf("\nWASN'T FOUND BY CUSTOMER: %d\n",j+1);
			        }
			        else if(catalog[i].customerNotSupportedCtr[j]>=5)  
			        {
				        printf("\nwasn't found by customer: %d\n",j+1);   
				        break;
			        }	
		        }
            }
            printf("\nTOTAL ORDERS: %d\n", ordersCtr);
	        printf("TOTAL SUCCESSFUL ORDERS: %d\n", orderSuccessfulCtr);
	        printf("TOTAL UNSUCCESSFUL ORDERS: %d\n", orderUnsuccessfulCtr);
	        printf("TOTAL TURNOVER: %.2f\n", totalTurnover);
        }
    }
    return 0;
}