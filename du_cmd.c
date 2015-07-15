/*
 * @file                : du command
 * @brief               : This is the source code for implementing the
                          du command   
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */

#include<sys/stat.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#define SUCCESS 1 
int main(int argc,char **argv)
{
	struct stat info;
	char buffer[20];
	buffer[0]='\0';
	int i;
	DIR *op_dir;
	struct dirent *rd_dir;
		
			 /* files in current directory*/
		if(argc==1) {						
			op_dir=opendir(".");				
			while((rd_dir=readdir(op_dir))!=NULL) {
				stat(rd_dir->d_name,&info);
				i=info.st_blocks/2;
				printf(" %5d         %s\n",i,rd_dir->d_name);
	
			}
			closedir(op_dir);
		}

			 /* files in current directory*/	
		if((argc==2) && (argv[1][0]!='.')) {
			op_dir=opendir(".");
				while((rd_dir=readdir(op_dir))!=NULL) {
					if(strcmp(rd_dir->d_name,argv[1])==0) {
						stat(rd_dir->d_name,&info);
						strcat(buffer,rd_dir->d_name);
						i=info.st_blocks/2;
						printf(" %5d       %s\n",i,buffer);
						buffer[0]='\0';
					}
				}
			closedir(op_dir);
		}
			
			 /* files in the given  directory*/
		if((argc==2) && (argv[1][1]=='/')) {
			op_dir=opendir(argv[1]);
				while((rd_dir=readdir(op_dir))!=NULL) {
					stat(rd_dir->d_name,&info);
				        strcat(buffer,argv[1]);
                   			strcat(buffer,"/");
					strcat(buffer,rd_dir->d_name);
					i=info.st_blocks/2;
					printf("%5d            %s\n",i,buffer);
					buffer[0]='\0';
				}
			closedir(op_dir);	
		}
		
	return SUCCESS;
}		

	
	

Wed May 21 17:44:44 2014
