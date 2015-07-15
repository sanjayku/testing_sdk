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

int main()
{
	int fd_read=0,fd_write=0;
	
		
		if((fd_read=open("/dev/chdevice",O_RDWR))==-1) {
			printf(" error : unable to find\n");
			return -1;
		}
	write(fd_read,"Hello",5);
	

		char buf[5];
	 
	read(fd_write,buf,sizeof(buf));	/* writing the data from buffer to new file */
	printf(" %s",buf);
	close(fd_read);
	return 1;
}

