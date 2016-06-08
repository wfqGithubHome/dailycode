#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

#define CGI_FORM_STRING                                 1
#define WIFI_MAX_ADMIN                                  3

#define WIFI_ADMIN_USERNAME_MAX_LEN                     6
#define WIFI_ADMIM_PASSWD_MAX_LEN                       8

#define WIFI_USER_USERNAME_LENGTH                       6
#define WIFI_USER_PASSWD_LENGTH                         4

#define WIFI_AUTH_USER_GROUP_NAME                       "user"
#define WIFI_AUTH_ADMIN_GROUP_NAME                      "admin"

char * flash_route_xmlconf = "/flash/etc/config.xml";
char * working_route_xmlconf = "/etc/config.xml";

extern int check_ipaddr(const char *ipaddr);
extern void log_debug(const char* fmt, ...);
extern int get_route_xml_node(const char * configfile,const int nodetype,void ** buf, int * out_size);
extern int free_xml_node(void ** buf);
extern int check_string_digit(char *str);
extern int copy_xmlconf_file(const char * srcfile, const char * dstfile);

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


int check_wifi_admin_username(const char *username){
      char c;

      if(!*username) 
            return -1;

      if(strlen(username) > WIFI_ADMIN_USERNAME_MAX_LEN)
            return -1;

      while(*username){
            c = *username;
            if(!isalnum(c) || (c == ' '))
                return -1;
            username++;
      }

      return 0;
}

int check_wifi_admin_passwd(const char *passwd){
      char c;

      if(!*passwd) 
            return -1;

      if(strlen(passwd) > WIFI_ADMIM_PASSWD_MAX_LEN)
            return -1;

      while(*passwd){
            c = *passwd;
            if(!isalnum(c) || (c == ' '))
                return -1;
            passwd++;
      }

      return 0;
}

int check_admin_format(char *admin){
      char s[STRING_LENGTH] = {0};
      char *item = ":";
      int num = 0,sign = 0;
      strncpy(s,admin,STRING_LENGTH);

      char *p = s;
      char *point = NULL;
      while(p){
            while((point = strsep(&p,item))){
                  sign++;
                  if((++num) > 2)
                        return -1;
                  if(sign == 1){
                        if(check_wifi_admin_username(point) != 0)
                                return -1;
                  }
                  else if(sign == 2){
                        if(check_wifi_admin_passwd(point) != 0)
                                return -1;
                  }
            }
      }

      if(sign !=2)
            return -1;
      return 0;
}

int check_wifi_admins_format(char *admins){
      char s[LONG_STRING_LENGTH] = {0};
      char admin[STRING_LENGTH] = {0};
      char *item = ";";
      int num = 0;
      strncpy(s,admins,LONG_STRING_LENGTH);

      char *p = s;
      char *point = NULL;

      while(p){
            while((point = strsep(&p,item))){      
                  if(num > WIFI_MAX_ADMIN)
                        return -1;
                  if(strlen(point) == 0){
                        return -1;
                  }
                  strncpy(admin,point,STRING_LENGTH);
                  if(check_admin_format(admin) != 0){
                        return -1;
                  }
            }
      }
      return 0;
}

static int do_add_group(char *groupname, int is_delete, int max_auth_ip)
{
    int iserror = 0,nodelete = 0;
    char cmd[CMD_LENGTH] = {0};

    if(is_delete){
        nodelete = 0;
    }else{
        nodelete = 1;
    }

    snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0  -A  -g  %s  -D  %d  --maxnum  %d",groupname,nodelete,max_auth_ip);
    do_wifi_command(cmd);
    return iserror;
}

static int do_add_account(char *groupname,char *username,char *password,int is_delete,int max_auth_ip)
{
    int iserror = 0,nodelete;
    char cmd[512] = {0};

    if(is_delete){
        nodelete = 0;
    }else{
        nodelete = 1;
    }

    snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -A  -u  %s -p  %s -g %s  -D %d  --maxnum  %d",
                        username,password,groupname,nodelete,max_auth_ip);
    do_wifi_command(cmd);
    return iserror;
}

static int do_add_admins(char *accounts)
{
    int iserror = 0;
    char admin[STRING_LENGTH] = {0};
    char username[STRING_LENGTH] = {0};
    char password[STRING_LENGTH] = {0};
    char *item = ";";

    char *p = accounts;
    char *point = NULL;

    while(p){
            while((point = strsep(&p,item))){      
                  strncpy(admin,point,STRING_LENGTH);
                  sscanf(admin,"%[0-9|a-z|A-Z]:%[0-9|a-z|A-Z]",username,password);
                  do_add_account(WIFI_AUTH_ADMIN_GROUP_NAME,username,password,0,0);
                  memset(username,0,sizeof(username));
                  memset(password,0,sizeof(password));
            }
      }
    
    return iserror;
}

static void do_add_redirect_url(void)
{
    int size,iserror = 0;
    char lan_ip[LITTER_LENGTH] = {0};
    char cmd[CMD_LENGTH] = {0};
    char *url_path = "/proc/sys/net/dpifilter/auth/auth_redirect_http_url";
    struct mgtcgi_xml_route_interface_lans *lan = NULL;
    
	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_INTERFACE_LANS, (void**)&lan, &size);
        
	if (NULL == lan){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

    strncpy(lan_ip,lan->ip,LITTER_LENGTH-1);
    snprintf(cmd,sizeof(cmd),"echo \"%s/AC/wifi/login.html\" > %s 2>/dev/null",lan_ip,url_path);
    do_wifi_command(cmd);

ERROR_EXIT:
    if(lan){
            free_xml_node((void*)&lan);
      }
    return;

}

void wifi_pool_add_save(void){
      int old_size,new_size,id,iserror = 0;
      int ret_value,old_num,new_num,enable;
      char startip[STRING_LENGTH] = {0};
      char endip[STRING_LENGTH] = {0};
      char comment[STRING_LENGTH] = {0};
      char enable_str[DIGITAL_LENGTH] = {0};
      char cmd[CMD_LENGTH] = {0};
      struct in_addr f_ip,e_ip;
      struct mgtcgi_xml_wifis  *old_wifis = NULL;
      struct mgtcgi_xml_wifis  *new_wifis = NULL;
	
#if CGI_FORM_STRING
      cgiFormString("startip", startip, sizeof(startip));
      cgiFormString("endip", endip, sizeof(endip));
      cgiFormString("comment", comment, sizeof(comment));
      cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

      if((strlen(startip) <= 0) ||
        (strlen(endip) <= 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }
      if (inet_aton(startip, &f_ip) == 0){//错误返回0
		  iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
	  }
      if (inet_aton(endip, &e_ip) == 0){//错误返回0
		  iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
	  }

       if (ntohl(f_ip.s_addr) >= ntohl(e_ip.s_addr)){
          iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
      }

      if((check_ipaddr(startip) !=0) || (check_ipaddr(endip)) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&old_wifis,&old_size);
      if ( NULL == old_wifis){
            iserror = MGTCGI_READ_FILE_ERR;
	     goto ERROR_EXIT;
      }

      old_size = sizeof(struct mgtcgi_xml_wifis) +
                        old_wifis->pool_num * sizeof(struct mgtcgi_xml_wifi_address_pool);

      enable = atoi(enable_str);
      old_num = old_wifis->pool_num;
      new_num = old_num + 1;
      id = new_num;
      new_size = sizeof(struct mgtcgi_xml_wifis) +
                        new_num * sizeof(struct mgtcgi_xml_wifi_address_pool);
      new_wifis = malloc(new_size);
      if(NULL == new_wifis){
            iserror = MGTCGI_READ_FILE_ERR;
            goto ERROR_EXIT;
      }
      memset(new_wifis,0,new_size);
      memcpy(new_wifis,old_wifis,old_size);

      new_wifis->pool_num = new_num;
      new_wifis->pinfo[old_num].id = id;
      new_wifis->pinfo[old_num].enable = enable;
      strncpy(new_wifis->pinfo[old_num].startip, startip, STRING_LENGTH);
      strncpy(new_wifis->pinfo[old_num].endip, endip, STRING_LENGTH);
      strncpy(new_wifis->pinfo[old_num].comment, comment, STRING_LENGTH);
      
      ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_WIFI_ID, new_wifis, new_size);
      if(ret_value < 0){
            iserror = MGTCGI_WRITE_FILE_ERR;
            goto ERROR_EXIT;
      }

      if(enable){
          snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -A --startip %s --endip %s",startip,endip);
          do_wifi_command(cmd);
      }

      ret_value = copy_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	  if (ret_value != 0){
		  iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		  goto ERROR_EXIT;
	  }
      
      printf("{\"iserror\":0,\"msg\":\"\"}");
      goto FREE_EXIT;
      
ERROR_EXIT:
      printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto FREE_EXIT;
FREE_EXIT:
      if(old_wifis){
            free_xml_node((void*)&old_wifis);
      }
      if(new_wifis){
            free_xml_node((void*)&new_wifis);
      }
      
      return;
}

void wifi_pool_edit_save(void){
      int size,index,iserror = 0;
      int ret_value,id,enable;
      char id_str[STRING_LENGTH] = {0};
      char startip[STRING_LENGTH] = {0};
      char endip[STRING_LENGTH] = {0};
      char comment[STRING_LENGTH] = {0};
      char enable_str[STRING_LENGTH] = {0};
      char cmd[CMD_LENGTH] = {0};
      struct in_addr f_ip,e_ip;
      struct mgtcgi_xml_wifis  *wifis = NULL;
	
#if CGI_FORM_STRING
      cgiFormString("id", id_str, sizeof(id_str));
      cgiFormString("startip", startip, sizeof(startip));
      cgiFormString("endip", endip, sizeof(endip));
      cgiFormString("comment", comment, sizeof(comment));
      cgiFormString("enable", enable_str, sizeof(enable_str));
#endif

      if((strlen(id_str) <= 0) ||
        (strlen(startip) <= 0) ||
        (strlen(endip) <= 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      if((check_ipaddr(startip) !=0) ||
        (check_ipaddr(endip) != 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      if (inet_aton(startip, &f_ip) == 0){//错误返回0
		  iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
	  }
      if (inet_aton(endip, &e_ip) == 0){//错误返回0
		  iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
	  }

       if (ntohl(f_ip.s_addr) >= ntohl(e_ip.s_addr)){
          iserror = MGTCGI_PARAM_ERR;
          goto ERROR_EXIT;
      }

      id =atoi(id_str);
      enable = atoi(enable_str);
      get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&wifis,&size);
      if ( NULL == wifis){
            iserror = MGTCGI_READ_FILE_ERR;
	     goto ERROR_EXIT;
      }

      for(index = 0; index < wifis->pool_num; index++){
            if(id == wifis->pinfo[index].id){
                  wifis->pinfo[index].enable = enable;
                  strncpy(wifis->pinfo[index].startip, startip, STRING_LENGTH);
                  strncpy(wifis->pinfo[index].endip, endip, STRING_LENGTH);
                  strncpy(wifis->pinfo[index].comment, comment, STRING_LENGTH);
            }
      }

      ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_WIFI_ID, (void*)wifis, size);
      if(ret_value < 0){
            iserror = MGTCGI_WRITE_FILE_ERR;
            goto ERROR_EXIT;
      }

      if(enable){
          snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -A --startip %s --endip %s",startip,endip);
      }else{
          snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -A --startip 0 --endip 0");
      }
      do_wifi_command(cmd);

      ret_value = copy_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	  if (ret_value != 0){
		  iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		  goto ERROR_EXIT;
	  }
      
      printf("{\"iserror\":0,\"msg\":\"\"}");
      goto FREE_EXIT;
      
ERROR_EXIT:
      printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto FREE_EXIT;
FREE_EXIT:
      if(wifis){
            free_xml_node((void*)&wifis);
      }
      
      return;
}

void wifi_basic_edit_save(void){
      int size,ret_value,iserror = 0;
      int oldenable,newenable,pc_time,max_time,share_num;
      char oldenable_str[STRING_LENGTH] = {0};
      char newenable_str[STRING_LENGTH] = {0};
      char pc_time_str[STRING_LENGTH] = {0};
      char max_time_str[STRING_LENGTH] = {0};
      char share_num_str[STRING_LENGTH] = {0};
      char admin_str[LONG_STRING_LENGTH] = {0};
      char cmd[CMD_LENGTH] = {0};
      struct mgtcgi_xml_wifis  *wifis = NULL;
	
#if CGI_FORM_STRING
      cgiFormString("oldenable", oldenable_str, sizeof(oldenable_str));
      cgiFormString("newenable", newenable_str, sizeof(newenable_str));
      cgiFormString("pc_discover_time", pc_time_str, sizeof(pc_time_str));
      cgiFormString("max_time", max_time_str, sizeof(max_time_str));
      cgiFormString("share_num", share_num_str, sizeof(share_num_str));
      cgiFormString("admins", admin_str, sizeof(admin_str));
#endif

      if((strlen(pc_time_str) == 0) ||
        (strlen(max_time_str) == 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      if(check_wifi_admins_format(admin_str) != 0){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      if((check_string_digit(pc_time_str) != 0) ||
        (check_string_digit(max_time_str) != 0)){
            iserror = MGTCGI_PARAM_ERR;
            goto ERROR_EXIT;
      }

      oldenable = atoi(oldenable_str);
      newenable = atoi(newenable_str);
      pc_time = atoi(pc_time_str);
      max_time = atoi(max_time_str);
      share_num = atoi(share_num_str);
      get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&wifis,&size);
      if ( NULL == wifis){
            iserror = MGTCGI_READ_FILE_ERR;
	     goto ERROR_EXIT;
      }

      wifis->enable = newenable;
      wifis->pc_discover_time = pc_time;
      wifis->max_time = max_time;
      wifis->share_num = share_num;
      strncpy(wifis->admins,admin_str,LONG_STRING_LENGTH);

      ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_WIFI_ID, (void*)wifis, size);
      if(ret_value < 0){
            iserror = MGTCGI_WRITE_FILE_ERR;
            goto ERROR_EXIT;
      }

      if(oldenable != newenable){
          snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -e %d",newenable);
          do_wifi_command(cmd);
      }

      if(newenable){
           do_add_group(WIFI_AUTH_USER_GROUP_NAME,1,share_num);
           do_add_group(WIFI_AUTH_ADMIN_GROUP_NAME,0,0);
           do_add_redirect_url();
           do_add_admins(admin_str);
      }

      ret_value = copy_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	  if (ret_value != 0){
		  iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		  goto ERROR_EXIT;
	  }
      
      printf("{\"iserror\":0,\"msg\":\"\"}");
      goto FREE_EXIT;
      
ERROR_EXIT:
      printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto FREE_EXIT;
FREE_EXIT:
      if(wifis){
            free_xml_node((void*)&wifis);
      }
      
      return;
}

void wifi_pool_remove_save(void){
      int iserror=0,index=0,size=0,ret_value=0,id=0;
      int new_size=0,new_index=0,old_num=0,new_num=0;
      char id_str[STRING_LENGTH]={"0"};
      char cmd[CMD_LENGTH] = {0};
      struct mgtcgi_xml_wifis  *old_wifis = NULL;
      struct mgtcgi_xml_wifis  *new_wifis = NULL;

#if CGI_FORM_STRING
      cgiFormString("id", id_str, sizeof(id_str));
#endif 
	
      if(strlen(id_str) <= 0){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
      }
      id = atoi(id_str);
      if (!(0 <= id && id <= 65535)){
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
      }
	
      get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&old_wifis,&size);
      if (NULL == old_wifis){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
      }
	
      old_num = old_wifis->pool_num;
      new_num = old_num - 1;	
	
      new_size = sizeof(struct mgtcgi_xml_wifis) + 
                        new_num * sizeof(struct mgtcgi_xml_wifi_address_pool);
	
      new_wifis = (struct mgtcgi_xml_wifis *)malloc(new_size);
      if (NULL == new_wifis){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
      }
	
      memset(new_wifis, 0, new_size);

      new_wifis->enable = old_wifis->enable;
      new_wifis->pc_discover_time = old_wifis->pc_discover_time;
      new_wifis->max_time = old_wifis->max_time;
      new_wifis->share_num = old_wifis->share_num;
      strncpy(new_wifis->admins,old_wifis->admins,LONG_STRING_LENGTH);

      new_wifis->pool_num = new_num;
      iserror = -1;
      for (new_index=0,index=0; index < old_wifis->pool_num; index++){
		if (old_wifis->pinfo[index].id == id){
			iserror = 0;
			continue;
		}

		if (new_index < old_wifis->pool_num){
			new_wifis->pinfo[new_index].enable = old_wifis->pinfo[index].enable;
                     strncpy(new_wifis->pinfo[new_index].startip,old_wifis->pinfo[index].startip,STRING_LENGTH);
                     strncpy(new_wifis->pinfo[new_index].endip,old_wifis->pinfo[index].endip,STRING_LENGTH);
                     strncpy(new_wifis->pinfo[new_index].comment,old_wifis->pinfo[index].comment,STRING_LENGTH);
			new_wifis->pinfo[new_index].id = new_index + 1; //id与new_index相同
			new_index++;
		}
		
     }
	
      if (iserror != 0){
		iserror = MGTCGI_NO_TARGET_ERR;
		goto ERROR_EXIT;
      }

      ret_value = save_xml_node(MGTCGI_XMLTMP, MGTCGI_WIFI_ID, (void*)new_wifis, new_size);
      if (ret_value < 0){
		iserror = MGTCGI_WRITE_FILE_ERR;
		goto ERROR_EXIT;
      }

      snprintf(cmd,sizeof(cmd),"/usr/local/sbin/authctl -b 0 -A --startip 0 --endip 0");
      do_wifi_command(cmd);

      ret_value = copy_xmlconf_file(working_route_xmlconf, flash_route_xmlconf);
	  if (ret_value != 0){
		  iserror = MGTCGI_SAVE_FLASH_ERR; //保存到flash错误
		  goto ERROR_EXIT;
	  }
	
      printf("{\"iserror\":0,\"msg\":\"\"}");
      goto FREE_EXIT;
ERROR_EXIT:
      printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
      goto FREE_EXIT;
FREE_EXIT:
      if(old_wifis){
	      free_xml_node((void*)&old_wifis);
      }
      if(new_wifis){
	      free_xml_node((void*)&new_wifis);
      }
      return;
}

void wifi_add_save(void){
       int iserror = 0;
       char type[STRING_LENGTH] = {0};

#if CGI_FORM_STRING
       cgiFormString("type", type, sizeof(type));
#endif 

       if(strcmp(type,"pool") == 0){
             wifi_pool_add_save();
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
       return;
}

void wifi_edit_save(void){
       int iserror = 0;
       char type[STRING_LENGTH] = {0};

#if CGI_FORM_STRING
       cgiFormString("type", type, sizeof(type));
#endif 

       if(strcmp(type,"pool") == 0){
             wifi_pool_edit_save();
       }
       else if(strcmp(type,"basic") == 0){
             wifi_basic_edit_save();
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
       return;
}

void wifi_remove_save(void){
       int iserror = 0;
       char type[STRING_LENGTH] = {0};

#if CGI_FORM_STRING
       cgiFormString("type", type, sizeof(type));
#endif 

       if(strcmp(type,"pool") == 0){
             wifi_pool_remove_save();
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
       return;
}

void wifi_list_show(void){
    int index = 0, size=0, flags=0;
	int iserror=0;
	struct mgtcgi_xml_wifis * wifis = NULL;
	
	get_xml_node(MGTCGI_XMLTMP,MGTCGI_WIFI_ID,(void**)&wifis,&size);
	if ( NULL == wifis){
		iserror = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}

	printf("{");
       printf("\"pools\":[");
       for(; index < wifis->pool_num; index++){
             if(flags) printf(",");
             printf("{\"id\":%d,\"startip\":\"%s\",\"endip\":\"%s\",\"comment\":\"%s\",\"enable\":%d}",
                                wifis->pinfo[index].id,
                                wifis->pinfo[index].startip,
                                wifis->pinfo[index].endip,
                                wifis->pinfo[index].comment,
                                wifis->pinfo[index].enable);
             flags = 1;
       }
       printf("],");
       
       printf("\"enable\":%d,\"pc_discover_time\":%d,\"max_time\":%d,\"share_num\":%d,\"admins\":\"%s\"}",
                                wifis->enable,wifis->pc_discover_time,wifis->max_time,wifis->share_num,wifis->admins);
	goto FREE_EXIT;
    
	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto FREE_EXIT;
	FREE_EXIT:
        if(wifis){
		    free_xml_node((void*)&wifis);
        }
		return;
}

int cgiMain(void){
      char submit_type[STRING_LENGTH];
      int iserror=0;

      cgiFormString("submit", submit_type, sizeof(submit_type));

#if _MGTCGI_DEBUG_GET_
      printf("content-type:text/html\n\n");		
#else
      printf("content-type:application/json\n\n");	
#endif

      if (strcmp(submit_type,"list") == 0 ){//列表显示所有
          wifi_list_show();
      }	
      else if(strcmp(submit_type,"addsave") == 0){//添加"确定"
	      wifi_add_save();
      }
      else if(strcmp(submit_type,"editsave") == 0){//编辑"确定"
	      wifi_edit_save();
      }
      else if(strcmp(submit_type,"remove") == 0){//点击删除
	      wifi_remove_save();
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
