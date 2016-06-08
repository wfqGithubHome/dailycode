#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

int cgiMain(void)
{   
    char *pathname = "/etc/HWID";    //path of the file
    char buf[BUF_SIZE];               //buffer
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
    printf("content-type:text/html\n\n");
	printf("%s\n",buf);

	return 0;     //output data
}
