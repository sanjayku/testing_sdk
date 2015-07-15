/*
 * @file                : webserver.c
 * @brief               : This is the source code for implementing the
                          webserver of vvdn website.   
 * @author              : Sanjay Kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */


#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>

main(int argc,char **argv)
{
	struct sockaddr_in ser_addr,cli_addr;
	char *rdbuf,*wrbuf;
	int ser_sd,cli_len,new_sd;
	
	if((ser_sd = socket(AF_INET,SOCK_STREAM,0))<0) {  /* opening a socket in TCP/IP mode */
			perror("socket");
			return;
	}
	
	memset(&ser_addr,0,sizeof(ser_addr));
        ser_addr.sin_family=AF_INET;			/* setting the socket in  IPv6 and a port assigned as argv[1] user choice*/
        ser_addr.sin_addr.s_addr=inet_addr("0.0.0.0");
        ser_addr.sin_port=htons(atoi(argv[1]));
		
	if(bind(ser_sd,(struct sockaddr*)&ser_addr,sizeof(ser_addr)) < 0) {
		perror("bind");
		return;
	}
	printf(" server waiting for request \n");
	listen(ser_sd,4);				/* max 4 clients can access socket at a time */
	cli_len=sizeof(cli_addr);
	
				 	
	while(1) {
			if((new_sd=accept(ser_sd,(struct sockaddr*)&cli_addr,&cli_len)) != -1) { /* waiting for client to connect */
		
			
				printf(" client connected \n");
				webser(new_sd);
				
		}
					
	}
}

webser(int new_sd)
{
	int i,n,fd;
	struct stat info;
	char *buf="HTTP/1.0 200 OK\nContent-Type:text/html\n\n"; /* buffer to be sent to make client understand as html */
	char rdbuf[400],wrbuf[100],*addr;

	
		bzero(rdbuf,400);
		bzero(wrbuf,100);
			
		if(recv(new_sd,rdbuf,sizeof(rdbuf),0) != -1) {
			printf(" %s \n",rdbuf);
			
			strcat(wrbuf,"./vvdntech.com");		/* seperating file name */

			strcpy(rdbuf,rdbuf+4);			
			addr=strchr(rdbuf,' ');
			n=addr-rdbuf;
			strncat(wrbuf,rdbuf,n);
			printf(" %s\n",wrbuf);			/* printing the file that is received */
			
			
			if((fd=open(wrbuf,O_RDONLY))==-1) {
			
				printf(" unable to find the file\n");
				return;
			}
			else {
				
		
				fstat(fd,&info);
				char var[info.st_size];

					
				read(fd,var,sizeof(var) );   /* reading the file */
			 	send(new_sd,buf,sizeof(buf),0);/* writing into socket for client */
				usleep(500);		
				send(new_sd,var,sizeof(var),0); /* writing into socket */
				usleep(500);
				bzero(var,sizeof(var));	
				
			}
		usleep(500);
		close(new_sd);
		}
		
}
