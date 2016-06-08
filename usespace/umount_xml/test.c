#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>


#define STRING_LENGTH  32
#define UMOUNT_XML_PROCESS_ID_FILE  "/var/run/umount_xml.pid"

int main(void)
{
  /*  int pp,pid;
    char pid_str[STRING_LENGTH] = {0};
    pp = open(UMOUNT_XML_PROCESS_ID_FILE,O_RDONLY);
    if(pp < 0){
        return 1;
    }
    read(pp,pid_str,sizeof(pid_str));
    pid = atoi(pid_str);
    printf("pid:%d\n",pid);

    kill(pid,SIGUSR1);
    sleep(5);
    kill(pid,SIGUSR2);*/

  printf("%d\n",PTHREAD_THREADS_MAX);
    return 0;
}
