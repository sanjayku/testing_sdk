/*
 * @file                : copy command
 * @brief               : This is the source code for implementing the
                          copy command   
 * @author              : Sanjay kumar U
 * @Copyright           : (c) 2014 , VVDN Technologies Pvt. Ltd.
                          Permission is hereby granted to everyone in VVDN Technologies
                          to use the Software without restriction, including without
                          limitation the rights to use, copy, modify, merge, publish,
                          distribute, distribute with modifications.helloooooooooooo
 */


#include<stdio.h>

void main (int argc,char **argv)
{
	FILE *fp;
	char ch;
	int i=0,cnt=0;
	if ((fp=fopen(argv[1],"r")) == NULL) {
		printf(" error : unable to find\n");
		exit (0);
	}
	while ((ch=fgetc(fp)) != EOF)
		cnt++;
		char str[cnt];
		rewind (fp);
		while ((ch=fgetc(fp)) != EOF) {
			if(i<cnt) {
				str[i]=ch;
				i++;
			}
		}
		fclose(fp);	

		fp=fopen(argv[2],"w");
		fwrite(str,1,sizeof(str),fp);
		fclose(fp);
		
	return;
}
	
