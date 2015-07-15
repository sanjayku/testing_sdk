#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include "wiced_time.h"
#include "wiced_security.h"
main()
{
	char input_str[50];
	char output_str[50];

	wiced_iso8601_time_t time;
	wiced_time_get_iso8601_time(wiced_iso8601_time_t &time);
	printf("%s\n",time);

	strcpy(input_str,"1002");
	strcpy(input_str,time);
	sha1(input_str,50,output_str);
	printf("%s\n",output_str);      /* PARTNERID_SHA */
	
	memset(input_str,'\0',50);
	memset(output_str,'\0',50);
	strcpy(input_str,"9493739");
	strcpy(input_str,time);
	sha1(input_str,50,output_str);
	printf("%s\n",output_str);			/* LOCALID_SHA */

}
