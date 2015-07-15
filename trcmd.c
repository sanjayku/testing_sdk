/*
 * @file                : tr command
 * @brief               : This is the source code for implementing the
                          tr command   
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
FILE *fp1,*fp2;
char ch,t1[20],t2[20],st1[20];
int i,j=0,cnt=0;
printf(" sanjay testin \n");
printf(" sanjay testin 2");
printf(" sanjay testin 3");
printf(" %d\n",argc);
	if(argc!=4 && argc!=5) {
       		 printf(" error : give ./a.out st1 st2 filename\n         give ./a.out st1 st2 filename targfile\n");
       		 return;
        }
	
sscanf(argv[1],"%s",t1);
sscanf(argv[2],"%s",t2);
printf(" %s\n",t1);
printf(" %s\n",t2);

fp1=fopen(argv[3],"r");
	if(fp1==NULL) {
	printf(" error fetching file \n");
	return -1;
	}
while((ch=fgetc(fp1))!=EOF)
	cnt++;
char str[cnt];
char str2[cnt];
rewind(fp1);

	
if(argc==4&&argc!=5) {					/* case for showing only change */
	while((ch=fgetc(fp1))!=EOF) {
					
		for(i=0;t1[i];i++) {
			if(ch==t1[i]) {
				ch=t2[i];
				break;
			}
		}
	printf("%c",ch);
	}	
	fclose(fp1);	
	
} else if(argc==5&&argc!=4) {				/*case for writing the change into another file*/

	fp2=fopen(argv[4],"w");
	while((ch=fgetc(fp1))!=EOF) {
		for(i=0;t1[i];i++) {
			if(ch==t1[i]) {
				ch=t2[i];
				break;
			}
		}
		if(j<cnt) {
			str[j]=ch;
			j++;
		} else break;
	}
	fclose(fp1);
	fwrite(str,1,sizeof(str),fp2);
	fclose(fp2);
fp2=fopen(argv[4],"r");
fread(str2,1,sizeof(str2),fp2);
puts(str2);
fclose(fp2);
}
printf("hi again changed");
printf("hi again changed");
}
