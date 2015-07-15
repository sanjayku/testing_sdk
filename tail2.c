/*
 * @file                : tail command
 * @brief               : This is the source code for implementing the
                          tail command   
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2012-2013 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.
 */
#include<stdio.h>

main(int argc,char **argv)
{
FILE *fp;
fp=fopen(argv[1],"r");
	if(fp==NULL) {
	printf(" error fetching file \n");
	return -1;
	}
int i,cnt=0;
char ch;
int a,b,c,d;
i=atoi(argv[2]);
while((ch=fgetc(fp))!=EOF) {
	if(ch=='\n')
	cnt++;	
	}
		if(cnt<i) {
			printf(" error : no of lines less than lines given to delete \n");
			exit(0);
			}
	rewind(fp);
	while(cnt!=i)
		{
		ch=fgetc(fp);
		if(ch=='\n')
		cnt--;
		}
	while((ch=fgetc(fp))!=EOF)
	printf("%c\n",ch);
	printf ("praba changed: ");
fclose(fp);					
				
}

