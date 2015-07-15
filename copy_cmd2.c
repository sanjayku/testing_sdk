/*
 * @file                : copy command
 * @brief               : This is the source code for implementing the
                          copy command   
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */
#include<string.h> 
#include<fcntl.h> 
#include<unistd.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>

int main(int argc,char **argv)
{
	int fd_read=0,fd_write=0;
	struct stat info;
		
		if((fd_read=open(argv[1],O_RDONLY))==-1) {
			printf(" error : unable to find\n");
			return -1;
		}

		fstat(fd_read,&info);	/* getting file information into info */
		char buf[info.st_size];
		read(fd_read,buf,sizeof(buf));  /* reading the file data into buf */ 
		close(fd_read);	

	fd_write=open(argv[2],O_CREAT|O_WRONLY); 
	write(fd_write,buf,sizeof(buf));	/* writing the data from buffer to new file */
	close(fd_write);
	return 1;
}

