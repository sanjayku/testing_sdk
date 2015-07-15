/*
 * @file                : watch_command.c
 * @brief               : This is the source code for implementing the
                          watch command in parent child.   
 * @author              : Sanjay Kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */

#include<stdio.h>
#include<unistd.h>
#include<time.h>

main(int argc,char **argv)
{
	int cnt,sec;
	int pid;
	time_t t=time(NULL);
	struct tm time;
	
	if(argc==2) {		/* case for default time */
		cnt=1;
		sec=2;
	} else 
	if(argc==4) {		/* case for setting time */
		if(strcmp(argv[1]),"-n") {
			sec=atoi(argv[2]);
			cnt=3;
		}
		}	
		
		 else {		/* case if no command is given */
			printf(" invalid arguments \n");
			exit(1);
		}
	
	while(1) {
	if(fork()==0) {				/* child performing watch command */
			system("clear")
			time=*localtime(&t);
			printf("nosecs:%d		                                            %d-%d-%d\n",sec,time.tm_year+1900,time.tm_mon+1,time.tm_mday);
			if((execlp(argv[cnt],argv[cnt],NULL))==-1) {
				printf(" error fetching command\n");
				break;
			}
			exit(0);
		}
	else {				/* parent waiting for child exit status */
		wait();
	 	sleep(sec);		
	
	}
	}
}
