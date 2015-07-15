/*
 * @file                : employee_database.c
 * @brief               : This is the source code for implementing the
                          employee database maintainig software.   
 * @author              : Sanjay Kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define SUCCESS 1

typedef struct student {
	char name[20];
	int id;
	char phone[12];
	char mail[20];
	struct student *next;
} EMP;

void add_db();
void del_db();
void print_db();
void read();
void update();
void sort();

EMP *head=NULL;	


int main()
{
	char choice;
	read();
	while(1){
		printf("        enter 'a' : add a database\n");
		printf("        enter 'd' : delete a database\n");
		printf("        enter 's' : sort database\n");
		printf("	enter 'p' : print database\n");
		printf("        enter 'u' : update\n");
		printf("        enter 'q' : quit\n");
		scanf(" %c",&choice);
		switch(choice) {
			case 'a': add_db();
				  break;
			case 'd': del_db();
				  break;
			case 'p': print_db();			  
				  break;
			case 'u': update();
				  break;
			case 's': sort();
				  break;
			case 'q': exit(0);
			default : printf("\n");
		}
	}
return SUCCESS;
}

/*function   	 :  add_db
 *return type	 :  void
 *parameters 	 :  void
 *brief          :  it takes head pointer and add data end of node
 */
void add_db()
{
	EMP *nu,*temp;
	temp=head;
	nu=malloc(sizeof(EMP));
	printf("enter the name of the student \n");
	scanf(" %s",nu->name);
	printf("enter student id integer \n");
	scanf(" %d",&nu->id);
	printf(" enter mobile no 10 characters\n");
	
	scanf(" %s",nu->phone);
	
	printf(" enter mail address \n");
	scanf(" %s",nu->mail);

 	 if(head==NULL) {
			head=nu;
			nu->next=NULL;
		
	  } else {
	    	while(temp->next!=NULL)
			temp=temp->next;
			temp->next=nu;
			nu->next=NULL;
		}
return;
}

/*function       :  del_db
 *return type    :  void
 *parameters     :  void
 *brief          :  it takes head pointer, searches the id and delete the node
 */

void del_db()
{
	EMP *temp,*del;
	temp=head;
	int del_id;
	printf(" enter the id that you want to delete \n");
	scanf(" %d",&del_id);
		if((temp!=NULL)&&(temp->id==del_id)) {
				if(temp->next!=NULL) {
					head=temp->next;
					free(temp);
				} else {
					printf(" only one in the list and getting deleted \n");
					free(temp);}
		} else {
			while (temp->next != NULL && temp->next->id != del_id) {
				temp=temp->next; 
			}
				if(temp->next==NULL) {
				printf(" id did not found \n");
				} else {
					if(temp->next->next != NULL) {
						del=temp->next;
						temp->next = temp->next->next;
						free(del);
					} else {
						del=temp->next;
						free(del);
						temp->next=NULL; 
						}
		
				}	
		}
	return;
}


/*function       :  print_db
 *return type    :  void
 *parameters     :  void
 *brief          :  it takes head pointer and prints all its nodes to terminal
 */

void print_db()
{
	EMP *temp;
	temp=head;
	if(temp) {
		while(temp) {
			printf(" name  :%s\n",temp->name);
			printf("   id  :%d\n",temp->id);
			printf("phone  :%s\n",temp->phone);
			printf(" mail  :%s\n",temp->mail);
			printf("\n");
			temp=temp->next;
		}
	} else printf("nothing in the list \n");
	return;
}

/* function       :  read()
 * return type    :  void
 * parameters     :  void
 * brief          :  it takes head pointer, open a file ,read them and add nodes to head 
 */


void read()
{
	FILE *emp_list;
	EMP buf,*temp,*nu;
	if((emp_list = fopen("datafile.txt","r"))==NULL)
		emp_list=fopen("datafile.txt","w+");
		while((fread(&buf,sizeof(EMP),1,emp_list)) != NULL) {
			        nu=malloc(sizeof(EMP));
		    		memcpy(nu,&buf,sizeof(EMP));
			  	nu->next =NULL;
			        if(head==NULL) {
   				          head=nu;
            				  temp=nu;
				} else {
          			   temp->next=nu;
        		 	   temp=nu;
           	    }	
        }
	fclose(emp_list);
	return;
}

/* function       :  update
 * return type    :  void
 * parameters     :  void
 * brief          :  it takes head pointer,open a file to write and update all nodes in file
 */

void update()
{
	FILE *emp_list;
	EMP *temp;
	temp=head;
	emp_list=fopen("datafile.txt","w");
		while(temp) {
			fwrite(temp,sizeof(EMP),1,emp_list);
			temp=temp->next;
		}
	fclose(emp_list);
	return;
}

/*function       :  sort
 *return type    :  void
 *parameters     :  void
 *brief          :  it takes head pointer and sort the nodes in ascending order of id
 */

void sort()
{
	EMP *temp1,*temp2,res;
	temp1=head;
	
	
		while(temp1->next) {
			temp2=temp1->next;
			printf(" %d",temp2->id);
			while(temp2) {
				if(temp1->id > temp2->id) {
					printf(" %d", temp1->id);
					res.id= temp1->id;
                        	        strcpy(res.name,temp1->name);
                               		strcpy(res.phone,temp1->phone);
                      		        strcpy(res.mail,temp1->mail);

	
					temp1->id= temp2->id;
					strcpy(temp1->name,temp2->name);
					strcpy(temp1->phone,temp2->phone);
					strcpy(temp1->mail,temp2->mail);
			
					temp2->id=res.id;
        	              		strcpy(temp2->name,res.name);
          			        strcpy(temp2->phone,res.phone);
                     			strcpy(temp2->mail,res.mail);
					temp2=temp2->next;
					} else {
						temp2=temp2->next;
				}	
	
			}	
			temp1=temp1->next;
		}
	temp1=head;
	while(temp1) {
		 printf(" name  :%s\n",temp1->name);
                 printf("   id  :%d\n",temp1->id);
                 printf("phone  :%s\n",temp1->phone);
                 printf(" mail  :%s\n",temp1->mail);
                 printf("\n");
                 temp1=temp1->next;
	}
	return;
}
