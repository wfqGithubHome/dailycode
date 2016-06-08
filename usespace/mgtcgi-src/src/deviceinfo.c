#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

int cgiMain(void)
{   
    char *pathname = "/etc/HWID";    //path of the file
    char buf[BUF_SIZE];               //buffer
	char arry[] = "\"acNum\"";
	FILE *f = NULL;    //pointer of the file
	int n;
	int sign;

    memset(buf,'\0',BUF_SIZE);

	f = fopen(pathname,"r");
	if(NULL == f)
	{
		perror("Open file failed!\nerror message");
		goto next;
	}

	n = fread(buf,sizeof(char),BUF_SIZE,f);
	if (n != strlen(buf))
	{
		perror("Read file failed!\nerror message");
		goto next;
	}
	if(buf[strlen(buf) - 1] == '\n')
	{
	       buf[strlen(buf) - 1] = '\0';
	}

	sign = fclose(f);
	if (sign != 0)
	{
		perror("Close file failed!\nerror message");
		goto next;
	}
	
next:
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");
#else
	printf("content-type:application/json\n\n");	
#endif
	printf("{%s:\"%s\"}\n",arry,buf);

	return 0;     //output data
}
