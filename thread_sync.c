/*
 * @file                : 27 pthread synchronisation
 * @brief               : This is the source code for implementing the
                          27 pthreads A-Z and '\n'.   
 * @author              : Sanjay Kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */


#include"headers2.h"
#include<time.h>
#include<sys/shm.h>

pthread_t thread1,thread2[26];
pthread_mutex_t lock;
key_t key;

void *write_thread(void *);
void *read_thread(void *);
int shmid;
char *shm ,*shm_copy ;
static char buf=64;
static int check=0;

void *write_thread(void *p)
{
	char buf_copy;
		 if(buf<=90) {
			if(buf==90) {
				buf_copy='\n';	
				buf=64;			
			}
			else {
				buf++;
				buf_copy=buf;
			}
		}
		else buf=64;			
		
		char *ret=&buf_copy;
		printf("reading %c\n",buf_copy);
				
		while(1) {				
			if(buf_copy==(65+check))  {	/* all threads waiting for this condition */

				shm_copy=shm;
				*shm_copy=buf_copy;	/* writing into shared memory by appr thread */
				printf("  writing %c  ",*shm_copy);
				pthread_create(&thread1,NULL,read_thread,NULL); 
				pthread_exit((void *)ret);
		
			}
			else if(check==26) {
				shm_copy=shm;
				*shm_copy='\n';
				pthread_create(&thread1,NULL,read_thread,NULL);
				pthread_exit((void *)ret);
			}
		}
}


void *read_thread(void *p)
{
FILE *file_ptr;
int i;
char *c;
	file_ptr=fopen("datafile.txt","a");
	shm_copy=shm;
	fputc(*shm_copy,file_ptr);  /* writing into file from shared memory */
	fclose(file_ptr);
	check++;
	printf(" %d\n",check);
	
	
}


main()
{
	int i=0,j=0,count;
	char *accept;
	key=30;

	if((shmid=shmget(key,1,IPC_CREAT|0666))<0) {		/* creating shared memory id */
		perror("shmget");
		exit(1);
	}

	if((shm=shmat(shmid,NULL,0))==NULL) {			/* getting shared memory segment */
		perror("shmat");
		exit(1);
	}
	printf(" enter no of times you wanna print A-Z\n");
	scanf(" %d",&count);
	for(i=0;i<count;i++) {
		for(j=0;j<27;j++) {
			pthread_create(&thread2[j],NULL,write_thread,NULL); /*  27 write threads writing a character
										 each and waiting for read thread  */
																		}
		for(j=0;j<27;j++) {
			pthread_join(thread2[j],(void **)&accept);	/* catching the exit status of all threads */
		}
			
	sleep(2);
	
	check=0;
	}				
		
//shmctl(shmid,IPC_RMID,0);
}
