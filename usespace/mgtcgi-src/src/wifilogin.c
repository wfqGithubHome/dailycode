#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "cgic.h"
#include "../include/mgtcgi_debug.h"
#include "../include/mgtcgi_xmlconfig.h"

#define WIFI_AUTH_PATH          "/usr/local/sbin/authctl"
#define USER_GROUP_NAME         "user"

#define PROCESS_OK         0
#define PROCESS_ERROR   -1

#define WIFI_USER_USERNAME_LENGTH                        12
#define WIFI_USER_PASSWD_LENGTH                          12

enum
{
        WIFI_USERNAME,
        WIFI_PASSWD,
};

void log_debug(const char* fmt, ...);
int get_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size);
int free_xml_node(void ** buf);

int do_get_wifi_command(const char *cmd, char *buffer, int lenght)
{
	FILE *pp = NULL;
	
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
    
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, lenght, pp);
		buffer[strlen(buffer)-1] = 0;
	}
	pclose(pp);
	return 0;
}

int do_wifi_command(char *cmd)
{
	int value=1;
	char buffer[DIGITAL_LENGTH]={0};
	FILE *pp = NULL;
	uid_t uid ,euid; 
	uid = getuid() ; 
	euid = geteuid(); 
	setreuid(euid, uid);
	
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, STRING_LENGTH, pp);
		buffer[strlen(buffer)-1] = 0;
		if (strcmp(buffer, "0") == 0)
			value = 0;
		else
			value = -1;
	}
	pclose(pp);
	return value;
}



int check_wifi_username(const char *username){
      char c;

      if(!*username) 
            return -1;

      if(strlen(username) > WIFI_USER_USERNAME_LENGTH)
            return -1;

      while(*username){
            c = *username;
            if(!isalnum(c) || (c == ' '))
                return -1;
            username++;
      }

      return 0;
}

int check_wifi_passwd(const char *passwd){
      char c;

      if(!*passwd) 
            return -1;

      if(strlen(passwd) > WIFI_USER_PASSWD_LENGTH)
            return -1;

      while(*passwd){
            c = *passwd;
            if(!isalnum(c) || (c == ' '))
                return -1;
            passwd++;
      }

      return 0;
}

int get_account(int type,char *data,int datalen){
      int iserror = PROCESS_OK;
      U64  p;
      
      srand((int)time(NULL));

      if(WIFI_USERNAME == type){
            p = (U64)(rand()%900000 + 100000.0);
      }else if(WIFI_PASSWD == type){
            p = (U64)(rand()%9000 + 1000.0);
      }else{
            iserror = PROCESS_ERROR;
            goto exit;
      }

      snprintf(data,datalen,"%lu",p);
exit:
      return iserror;
}

void wifi_account_login(void){
      int iserror = PROCESS_ERROR;
      char username[STRING_LENGTH] = {0};
      char passwd[STRING_LENGTH] = {0};
      char ip[STRING_LENGTH] = {0};
      char cmd[CMD_LENGTH] = {0};

      strncpy(ip,getenv("REMOTE_ADDR"),STRING_LENGTH);
      cgiFormString("username", username, sizeof(username));
      cgiFormString("passwd", passwd, sizeof(passwd));

      if((check_wifi_username(username) != 0) ||
            (check_wifi_passwd(passwd) != 0)){
            iserror = PROCESS_ERROR;
            goto error;
      };

      snprintf(cmd,sizeof(cmd),"%s -b 0 -l -u %s -p %s -a %s",WIFI_AUTH_PATH,username,passwd,ip);
      iserror = do_wifi_command(cmd);

      printf("{\"iserror\":%d,\"errormsg\":\"\"}",iserror);
      goto exit;
error:
      printf("{\"iserror\":%d,\"errormsg\":\"Parameters error\"}",iserror);
exit:
      return;
}

void wifi_account_status(void){
      int max_time,iserror = PROCESS_OK;
      char username[STRING_LENGTH] = {0};
      char passwd[STRING_LENGTH] = {0};
      char maxtime_str[STRING_LENGTH] = {0};
      char cmd[CMD_LENGTH] = {0};

      cgiFormString("username", username, sizeof(username));
      cgiFormString("passwd", passwd, sizeof(passwd));
      cgiFormString("max_time", maxtime_str, sizeof(maxtime_str));

      if((check_wifi_username(username) != 0) ||
            (check_wifi_passwd(passwd) != 0)){
            iserror = PROCESS_ERROR;
            goto error;
      }

      max_time = atoi(maxtime_str);
      if(max_time <= 0){
          max_time = 5;
      }
      snprintf(cmd,sizeof(cmd),"%s -b 0 -l -u %s -p %s -t %d",WIFI_AUTH_PATH,username,passwd,max_time);
      do_wifi_command(cmd);

      printf("{\"iserror\":%d,\"errormsg\":\"\"}",iserror);
      goto exit;
error:
      printf("{\"iserror\":%d,\"errormsg\":\"Parameters error\"}",iserror);
exit:
      return;
}

void wifi_accout_get(){
      int size,iserror = 0,index = 0;
      int discover_time,max_time,account_num;
      char username[STRING_LENGTH] = {0};
      char passwd[STRING_LENGTH] = {0};
      char ip[STRING_LENGTH] = {0};
      char cmd[LONG_STRING_LENGTH] = {0};
      char str[STRING_LENGTH] = {0};

      char first_ip[STRING_LENGTH] = {0};
      char end_ip[STRING_LENGTH] = {0};
      struct in_addr f_ip,e_ip,if_ip;

      struct mgtcgi_xml_wifis * wifis = NULL;
	
	  get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&wifis,&size);
	  if ( NULL == wifis){
		  iserror = MGTCGI_READ_FILE_ERR;
		  goto ERROR;
	  }

      if(wifis->enable == 0){
          iserror = PROCESS_ERROR;
          goto ERROR;
      }

      if(wifis->pool_num == 0){
          iserror = PROCESS_ERROR;
          goto ERROR;
      }

      index = wifis->pool_num;
      discover_time = wifis->pc_discover_time;
      max_time = wifis->max_time;
      account_num = wifis->share_num;
      strncpy(first_ip, wifis->pinfo[index-1].startip,sizeof(first_ip));
      strncpy(end_ip, wifis->pinfo[index-1].endip,sizeof(end_ip));

      strncpy(ip,getenv("REMOTE_ADDR"),STRING_LENGTH);

      if (inet_aton(first_ip, &f_ip) == 0){//¥ÌŒÛ∑µªÿ0
		  iserror = PROCESS_ERROR;
          goto ERROR;
	  }
      if (inet_aton(end_ip, &e_ip) == 0){//¥ÌŒÛ∑µªÿ0
		  iserror = PROCESS_ERROR;
          goto ERROR;
	  }
      if (inet_aton(ip, &if_ip) == 0){//¥ÌŒÛ∑µªÿ0
		  iserror = PROCESS_ERROR;
          goto ERROR;
	  }

      if ((ntohl(if_ip.s_addr) >= ntohl(f_ip.s_addr)) && (ntohl(if_ip.s_addr) <= ntohl(e_ip.s_addr))){
          iserror = PROCESS_ERROR;
          goto ERROR;
      }
      
      if ((get_account(WIFI_USERNAME,username,STRING_LENGTH) != 0) ||
           (get_account(WIFI_PASSWD,passwd,STRING_LENGTH) != 0)){
            iserror =  PROCESS_ERROR;
            goto ERROR;
      }

      snprintf(cmd,sizeof(cmd),"%s -b 0 -A -u %s -p %s -g %s -a %s -D 0 --maxnum %d",WIFI_AUTH_PATH,
                        username,passwd,USER_GROUP_NAME,ip,account_num);
      do_get_wifi_command(cmd,str,sizeof(str));
      if(strlen(str) > 1){
          sscanf(str,"%[0-9|a-z|A-Z]:%[0-9|a-z|A-Z]",username,passwd);
      }

      printf("{\"iserror\":%d,\"errormsg\":\"\",\"discover_time\":\"%d\",\"max_time\":\"%d\",\"data\":{\"username\":\"%s\",\"passwd\":\"%s\"}}",
                            iserror,discover_time,max_time,username,passwd);
      goto EXIT;
ERROR:
      printf("{\"iserror\":%d,\"errormsg\":\"\"}",iserror);
      goto EXIT;
EXIT:
      if(wifis){
            free_xml_node((void*)&wifis);
      }
      return;
}

int cgiMain(){
      char submit_type[STRING_LENGTH];
      int iserror=0;

      cgiFormString("submit", submit_type, sizeof(submit_type));

#if _MGTCGI_DEBUG_GET_
      printf("content-type:text/html\n\n");		
#else
      printf("content-type:application/json\n\n");	
#endif

      if (strcmp(submit_type,"get") == 0 ){//ªÒ»°’À∫≈√‹¬Î
          wifi_accout_get();
      }	
      else if(strcmp(submit_type,"status") == 0){//∑¢ÀÕ◊¥Ã¨–≈œ¢
	      wifi_account_status();
      }
      else if(strcmp(submit_type,"login") == 0){//µ«¬º
	      wifi_account_login();
      }
      else{
	     iserror = MGTCGI_PARAM_ERR;
	     goto ERROR_EXIT;
      }

      goto CGIC_EXIT;

ERROR_EXIT:
      printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto CGIC_EXIT;
CGIC_EXIT:
      return 0;
}
