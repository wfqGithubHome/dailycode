#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../include/mgtcgi_xmlconfig.h"
//include "../include/mgtcgi_debug.h"

#define UMOUNT_XML_PROCESS_ID_FILE  "/var/run/umount_xml.pid"
#define CFDEV_FILE_PATH             "/etc/CFDEV"

int do_check_command(const char *cmd, char *buffer, int lenght)
{
	FILE *pp = NULL;
	
	uid_t uid ,euid; 
	uid = getuid(); 
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


int check_cgifrom_digit(int value, int min, int max)
{
	int ret_value = 0;
	if (!(min <= value && value <= max))
		ret_value = -1;
	else
		ret_value = 0;

	return ret_value;
}

int check_string_digit(char *str)
{
    char c;

    if(str == NULL){
        return -1;
    }
    while(*str){
        c = *str;
        if((c < '0') || (c > '9'))
            return -1;
        str++;
    }

    return 0;
}

/************************修改名称检查***********************************/
int group_name_check(struct mgtcgi_xml_group_array *group, 
								const char *name, const char *old_name)
{
	//name用于重复性检查			重复返回2
	//old_name用于是否被使用检查	被使用返回1，正常返回0
	int index,i,ret_value=0;
	for (index=0; index < group->num; index++){
		if (strcmp(group->pinfo[index].group_name, name) == 0){//与组名相同
			ret_value = 2;
			break;
		}
		for(i=0; i< group->pinfo[index].num; i++){
			if (strcmp(group->pinfo[index].pinfo[i].name, old_name) == 0){//组中被使用
				ret_value = 1;
				break;
			}			
		}
	}

	return (ret_value);
}


int ipsession_name_check(struct mgtcgi_xml_ip_session_limits *ipsession,
					const char *name, const char *old_name, int check_id)
{
//返回值:0,没有被使用
//		 1,被使用，不能使用
//		 -1,参数错误
	int index,ret_value=0;
	if (1 == check_id){	//ip地址对象，ip地址对象组
		for (index=0; index < ipsession->global_limit_num; index++){
			if (strcmp(ipsession->p_globalinfo[index].addr, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(ipsession->p_globalinfo[index].dst, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if(2 == check_id){	//会话数对象
		for (index=0; index < ipsession->global_limit_num; index++){
			if (strcmp(ipsession->p_globalinfo[index].per_ip_session_limit, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(ipsession->p_globalinfo[index].total_session_limit, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else {
		ret_value = -1;
	}
	
	return (ret_value);
}

int firewall_name_check(struct mgtcgi_xml_firewalls *firewall,
					const char *name, const char *old_name, int check_id)
{
//返回值:0,没有被使用
//		 1,被使用，不能使用
//		 -1,参数错误
	int index,ret_value=0;
	if (1 == check_id){	//IP对象，ip对象组
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].src, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(firewall->pinfo[index].dst, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (2 == check_id){//时间对象，对象组
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].schedule, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (3 == check_id) {//vlan对象
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].vlan, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (4 == check_id) {//会话数对象
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].session_limit, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (5 == check_id) {//自定义协议对象，对象组
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].proto, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (6 == check_id){//进入流量对象
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].in_traffic, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (7 == check_id){//外出流量对象
		for (index=0; index < firewall->num; index++){
			if (strcmp(firewall->pinfo[index].out_traffic, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else {
		ret_value = -1;
	}

	return (ret_value);
}


int portmirror_name_check(struct mgtcgi_xml_port_mirrors *portmirror,
					const char *name, const char *old_name, int check_id)
{
//返回值:0,没有被使用
//		 1,被使用，不能使用
//		 -1,参数错误
	int index,ret_value=0;

	if (1 == check_id){//ip对象，对象组
		for (index=0; index < portmirror->num; index++){
			if (strcmp(portmirror->pinfo[index].src, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(portmirror->pinfo[index].dst, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else if (2 == check_id){ //自定义协议对象，对象组
		for (index=0; index < portmirror->num; index++){
			if (strcmp(portmirror->pinfo[index].proto, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else {
		ret_value = -1;
	}

	return (ret_value);
	
}


int dnspolicy_name_check(struct mgtcgi_xml_dns_policys *dnspolicy,
					const char *name, const char *old_name, int check_id)
{
//返回值:0,没有被使用
//		 1,被使用，不能使用
//		 -1,参数错误
	int index,ret_value=0;
	if (1 == check_id){//ip对象，对象组
		for (index=0; index < dnspolicy->num; index++){
			if (strcmp(dnspolicy->pinfo[index].src, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(dnspolicy->pinfo[index].dst, old_name) == 0){
				ret_value = 1;
				break;
			}
			if (strcmp(dnspolicy->pinfo[index].targetip, old_name) == 0){
				ret_value = 1;
				break;
			}
		}
		
	} else if (2 == check_id){//domain对象，对象组
		for (index=0; index < dnspolicy->num; index++){
			if (strcmp(dnspolicy->pinfo[index].domain_name, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else {
		ret_value = -1;
	}

	return (ret_value);
}


int httpdir_policy_name_check(struct mgtcgi_xml_http_dir_policys *httpdir_policy,
					const char *name, const char *old_name, int check_id)
{
//返回值:0,没有被使用
//		 1,被使用，不能使用
//		 -1,参数错误
	int index,ret_value=0;
	if (1 == check_id){//扩展名对象，对象组
		for (index=0; index < httpdir_policy->policy_num; index++){
			if (strcmp(httpdir_policy->p_policyinfo[index].extgroup, old_name) == 0){
				ret_value = 1;
				break;
			}
		}	
	} else if (2 == check_id){//服务器对象，对象组
		for (index=0; index < httpdir_policy->policy_num; index++){
			if (strcmp(httpdir_policy->p_policyinfo[index].servergroup, old_name) == 0){
				ret_value = 1;
				break;
			}
		}

	} else {
		ret_value = -1;
	}

	return (ret_value);
}

static int get_umount_xml_pid(int *pid)
{
    int reavl = 0;
    char pid_str[STRING_LENGTH] = {0};
    FILE *fp = NULL;

    fp = fopen(UMOUNT_XML_PROCESS_ID_FILE,"r");
    if(NULL == fp){
        reavl = -1;
        goto EXIT;
    }
    if(fread(pid_str,1,sizeof(pid_str),fp) == 0){
        reavl = -1;
        goto EXIT;
    }

    *pid = atoi(pid_str);

EXIT:
    if(fp){
        fclose(fp);
        fp = NULL;
    }
    return reavl;
}

static int get_cfdev(char *cfdev,int lenth)
{
    int travl = 0;
    int fp;

    fp = open(CFDEV_FILE_PATH,O_RDONLY);
    if(fp < 0){
        travl = -1;   
        goto ERROREXIT;
    }

    if(read(fp,cfdev,lenth) == -1){
        travl = -1;
        goto ERROREXIT;
    }
    fflush(NULL);
ERROREXIT:
    close(fp);
    return travl;
}

int check_flash_mount(void)
{
    int reavl = 0;
    int mount = 0;
    char cmd[512] = {0};
    char cfdev[STRING_LENGTH] = {0};
    char buf[STRING_LENGTH] = {0};
    
    if(get_cfdev(cfdev,sizeof(cfdev)) != 0){
        reavl = -1;
        goto EXIT;
    }
    cfdev[strlen(cfdev) - 1] = '\0';
    snprintf(cmd,sizeof(cmd),"/bin/mount -w |grep %s |grep -v grep |wc -l",cfdev);
    do_check_command(cmd,buf,sizeof(buf)-1);
    if(strlen(buf) == 0){
        reavl = -1;
        goto EXIT;
    }
    
    mount = atoi(buf);
    if(mount > 0){
        goto EXIT;
    }

    memset(cmd,0,sizeof(cmd));
    snprintf(cmd,sizeof(cmd),"/bin/mount /dev/%s /flash",cfdev);
    do_check_command(cmd,buf,sizeof(buf)-1);

EXIT:
    return reavl;
}

void flush_timer(void)
{
    int pid = 0;
    int max_pid = 0;
    char cmd[STRING_LENGTH] = {0};
    char buffer[STRING_LENGTH] = {0};
    
    if(get_umount_xml_pid(&pid) != 0){
        goto EXIT;
    }

    snprintf(cmd,sizeof(cmd),"cat /proc/sys/kernel/pid_max");
    do_check_command(cmd,buffer,sizeof(buffer)-1);
    max_pid = atoi(buffer);

    if((pid > max_pid) || (pid <= 0)){
        goto EXIT;
    }

    kill(pid,SIGUSR1);

EXIT:
    return;
}

void start_timer(void)
{
    int pid = 0;
    int max_pid = 0;
    char cmd[STRING_LENGTH] = {0};
    char buffer[STRING_LENGTH] = {0};
    
    if(get_umount_xml_pid(&pid) != 0){
        goto EXIT;
    }

    snprintf(cmd,sizeof(cmd),"cat /proc/sys/kernel/pid_max");
    do_check_command(cmd,buffer,sizeof(buffer)-1);
    max_pid = atoi(buffer);

    if((pid > max_pid) || (pid <= 0)){
        goto EXIT;
    }

    kill(pid,SIGUSR2);

EXIT:
    return;
}

