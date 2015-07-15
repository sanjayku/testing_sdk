/*
 * @file                : Testing USB configuration transmission and reception
 * @brief               : This is the source code for implementing the
                          USB setup and testing   
 * @author              : Sanjay Kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */


#include"headers.h"
#define SUCCESS 1

long get_baud(void);
char get_parity(void);
int set_size(int);
int set_parity(char);
int set_baud(long);

struct termios options;
int Qid;

struct msgbuf {
		
		long mtype ;
		long baudrate;
		char parity;
		int size;
	
	} ;


long get_baud()
{
	char ch;
	long baud;
	printf(" enter 1 : B50	50 baud\n");
	printf(" enter 2 : B75	75 baud\n");
	printf(" enter 3 : B110	110 baud\n");
	printf(" enter 4 : B134	134.5 baud\n");
	printf(" enter 5 : B150	150 baud\n");
	printf(" enter 6 : B200	200 baud\n");
	printf(" enter 7 : B300	300 baud\n");
	printf(" enter 8 : B600	600 baud\n");
	printf(" enter 9 : B1200 	1200 baud\n");
	printf(" enter a : B1800	1800 baud\n");
	printf(" enter b : B2400	2400 baud\n");
	printf(" enter c : B4800	4800 baud\n");
	printf(" enter d : B9600	9600 baud\n");
	printf(" enter e : B19200	19200 baud\n");
	scanf(" %c",&ch);
		switch(ch) {
			case '1' : baud= 50;
					break;
			case '2' : baud= 75;
					break;
			case '3' : baud= 110;
					break;
			case '4' : baud= 134;
					break;
			case '5' : baud= 150;
					break;
			case '6' : baud= 200;
					break;
			case '7' : baud= 300;
					break;
			case '8' : baud= 600;
					break;
			case '9' : baud= 1200;
					break;
			case 'a' : baud= 1800;
					break;
			case 'b' : baud= 2400;
					break;
			case 'c' : baud= 4800;
					break;
			case 'd' : baud= 9600;
					break;
			case 'e' : baud= 19200;
					break;
			default : printf(" \n");
		}
	return baud;
}

char get_par()
{
	char par;
	printf(" enter e for even parity \n");
	printf(" enter o for odd parity \n");
	printf(" enter n for no parity \n");
	scanf(" %c",&par);
	return par;
}
int get_size()
{
	int size;
	printf(" enter the bit size 5/6/7/8 \n");
	scanf(" %d",&size);
	return size;
}




int set_parity(char ch)
{
		if(ch=='e') {
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_cflag &= ~CSTOPB;
		}
		else if(ch=='o') {
			options.c_cflag |= PARENB;
			options.c_cflag &= ~CSTOPB;
			
		}
		else if(ch=='n') {
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			
		}
	return SUCCESS;
}
	
int set_size(int size)
{
	if(size==5) {
		options.c_cflag |= CS5 ;
	} 
	else if(size==6) {
		options.c_cflag |= CS6 ;
	}
	else if(size==7) {
		options.c_cflag |= CS7 ;
	}
	else if(size==8) {
		options.c_cflag |= CS8 ;
	}
	return SUCCESS;
}

int set_baud(long baudrate)
{
	if(baudrate == 50 ){
		cfsetispeed(&options,B50);
		cfsetospeed(&options,B50);
	}
	else if(baudrate == 75 ) {
		cfsetispeed(&options,B75);
		cfsetospeed(&options,B75);
	}
	else if(baudrate == 110 ) {
		cfsetispeed(&options,B110);
		cfsetospeed(&options,B110);
	}
			
	else if(baudrate == 200 ) {
		cfsetispeed(&options,B200);
		cfsetospeed(&options,B200);
	}

	else if(baudrate == 300 ) {
		cfsetispeed(&options,B300);
		cfsetospeed(&options,B300);
	}
	
	else if(baudrate == 600 ) {
		cfsetispeed(&options,B600);
		cfsetospeed(&options,B600);
	}
	
	else if(baudrate == 1200 ) {
		cfsetispeed(&options,B1200);
		cfsetospeed(&options,B1200);
	}
	
	else if(baudrate == 1800 ) {
		cfsetispeed(&options,B1800);
		cfsetospeed(&options,B1800);
	}
	return SUCCESS;
}



main()
{
	struct msgbuf buf;
	struct termios options;

	Qid=msgget(5,IPC_CREAT|0660);
	if(fork()==0) {
		
		buf.mtype=1;
		buf.baudrate=get_baud();
		printf("baud %ld\n",buf.baudrate);
		buf.parity = get_par();
		buf.size = get_size();

			if(msgsnd(Qid,&buf,sizeof(buf),0)>0)
				printf(" details are sent in msg queue to termios \n");
		exit(0);	/* exit status of child1 also is  process1 */
	}
	else {
		int status;
		struct termios options;
		struct msgbuf buf;
		int fd_usb;
		
		wait(&status); /* parent  catching the child1 exit status */
		printf(" enter the process 2 %d \n",status>>8);
	
		if(fork()==0) {  		/* process 2 */
			buf.mtype=1;
			msgrcv(Qid,&buf,sizeof(buf),buf.mtype,0);
			printf(" %d %c %ld\n",buf.size,buf.parity,buf.baudrate);
				if((fd_usb = open("/dev/ttyUSB0",O_RDWR|O_NOCTTY|O_NDELAY))==-1) {
					perror(" error");
					return;
				}
			tcgetattr(fd_usb,&options);			/* getting USB options into termios buffer options*/
			set_parity(buf.parity);			/* setting parity even odd no parity */
			set_size(buf.size);				/* setting size 5 /6/7/8 bits */
			set_baud(buf.baudrate); 		/* setting baud rate */
			options.c_cflag |= (CLOCAL|CREAD);				/* enable receiver and set local mode */
			options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);	/* enable RAW input */
			options.c_oflag &= ~OPOST	;					/* enable RAW output */
			options.c_iflag |= (INPCK|ISTRIP);				/* checking and stripping parity bit */
			options.c_iflag |= (IXON|IXOFF|IXANY);			/* enable software control */
			tcsetattr(fd_usb,TCSANOW,&options);
			close(fd_usb);
			exit(1); /* child 2 exiting */
		}
		 else {
		
		int status;
		wait(&status); /* parent receiving exit status and is  also process 3 */
		printf(" enter %d \n",status>>8);
		int fd_usb ,n;
		char wr_buf[20];
		char rd_buf[20];
		if((fd_usb = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY))==-1)
			printf(" error");
		
		while(1) {
			printf(" write data ....  \n");
			bzero(wr_buf,20);
			if((strcmp(wr_buf,"q") == 0)) {
				close(fd_usb);
				exit(2);
			}																
			gets(wr_buf);
			n=write(fd_usb,wr_buf,20);			/* writing into usb */
			if(n<0){ 
				printf(" error in writing \n");
				
			}
			printf(" reading from usb .....\n");
			
		sleep(1);
			__fpurge(stdin);
			n=read(fd_usb,rd_buf,20);
			
			if(n<0) {
				printf(" error in reading \n");
				
			}
			puts(rd_buf);					/* reading from usb */
			
		}
		}
}
}



