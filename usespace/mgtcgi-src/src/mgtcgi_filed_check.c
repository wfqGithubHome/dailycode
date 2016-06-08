#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include	<netinet/in.h>
#include	<sys/socket.h>
#include	<sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../include/mgtcgi_xmlconfig.h"
#include "../include/mgtcgi_debug.h"



/*
 * 检查weeks中是否包含了day中的字符串
 * 返回值:
 * 0 不包含
 * 1 包含
*/
int check_weeks(const char *weeks, const char *day)
{
	char *p = NULL;
	if ((NULL == weeks) || (NULL == day))
		return (0);
	p = strstr(weeks, day);
	if (NULL != p)
		return (1);
	else
		return (0);
}

int check_list_string_dot(const char *string, const char *dot)
{
	char *p;
	//char str[STRING_LIST_LENGTH];
	char * str=NULL;
	int dot_num = 0;
    str = (char *) malloc(STRING_LIST_LENGTH*sizeof(char));
    if(!str)
        return 0;
	strcpy(str, string);
	p=strtok(str,dot);
	if (NULL != p){
		dot_num++;
	}
	while((p=strtok(NULL,dot))){
		if (NULL != p){
			dot_num++;
		}
	}
    free(str);
    str= NULL;
	return (dot_num);
}

int mac_repair(char *mac, const int length, const char *dot)
{
	int i;
    if(!mac){
        return 1;
    }
    
	if (strcmp(dot, ":") == 0){
		for (i=0; i<length; i++){
			if (mac[i] == '-')
				mac[i] = ':';
		}
	}
	else{
		for (i=0; i<length; i++){
			if (mac[i] == ':')
				mac[i] = '-';
		}
	}
	return 0;
}
//MAC地址合法性检查
int check_mac_addr(const char *macaddr, const char *dot)
{
	char *strtmp = NULL;
	char mactmp[STRING_LENGTH] = {0};
	int hexnum = 0;
	
	if (strlen(macaddr) != 17)
		return -1;
	else
		strcpy(mactmp, macaddr);

	strtmp = strtok(mactmp, dot);
	hexnum = 0;	
	while(strtmp != NULL)
	{
		if (strlen(strtmp) != 2)
			return -1;
		
		if((strtmp[0] >= '0' && strtmp[0] <= '9') || 
			(strtmp[0] >= 'a' && strtmp[0] <= 'f') || 
			(strtmp[0] >= 'A' && strtmp[0] <= 'F')){
			if((strtmp[1] >= '0' && strtmp[1] <= '9') || 
				(strtmp[1] >= 'a' && strtmp[1] <= 'f') || 
				(strtmp[1] >= 'A' && strtmp[1] <= 'F')){
				hexnum++;
				strtmp = strtok(NULL, dot);
			}
			else{
				return -1;
			}
		}
		else{
			return -1;
		}
	}
	
	if (hexnum != 6)
		return -1;
	else
		return 0;
}


//int inet_aton(const char *string, struct in_addr *addr); 

/* 检查IP地址分隔符个数，确定IP地址类型
 * 输入值:const char *ipaddr 	IP地址指针
 *        const char *dot       分隔符类型指针
 * 输出值: 分隔符个数
*/

int check_ipaddr_dot(const char *ipaddr, const char *dot)
{
        int num=0;
        while(*ipaddr != '\0')
        {
                if (*ipaddr++ == *dot)
                        num++;
        }
        return (num);
}

/* 检查IP地址类型
 * 输入值:const char *ipaddr
 * 输出值: -1 IP地址类错误，或者不识别的类型
 *          1  主机地址 192.168.1.1
 *          2  网络地址 192.168.1.0/24
 *          3  IP段     192.168.1.1-192.168.1.254
*/
int check_ipaddr_type(const char *ipaddr)
{
	int netdot_count = 0;
	int pooldot_count = 0;
	char netaddr_dot[]="/";
	char pooladdr_dot[]="-";
	char hostip_dot[]=".";

	if (ipaddr == NULL)
	{
		return (-1);
	}
	
	netdot_count = check_ipaddr_dot(ipaddr, netaddr_dot);
	pooldot_count = check_ipaddr_dot(ipaddr, pooladdr_dot);

	if ( netdot_count == 0 && pooldot_count == 0)
	{
		netdot_count = check_ipaddr_dot(ipaddr, hostip_dot);
		if (netdot_count != 3)
			return -1;
		else
			return 1;
	}
	else if (netdot_count == 1 && pooldot_count == 0)
	{
		return (2);
	}
	else if (netdot_count == 0 && pooldot_count == 1)
	{
		return (3);
	}
	else
	{
		return (-1);
	}
	
	
}

/*IP地址合法性检查
 *输入值:const char *ipaddr
 *       192.168.1.1，192.168.1.0/24,192.168.1.1-192.168.1.254为合法IP
 *输出值:0  IP地址合法
 *      -1  IP地址不合法
*/
int check_ipaddr(const char *ipaddr)
{
	int iptype = 0;
	int i=0,j=0,flag=0;
	int inet_result = 0;
	int return_val = -1;
	struct in_addr inp, inp2;

	//char netaddr_dot[]="/";
	//char pooladdr_dot[]="-";
	char hostip_dot[]=".";
	char ipaddr_head[STRING_LENGTH]={0};
	char ipaddr_tail[STRING_LENGTH]={0};
	char check_ipaddr[STRING_LENGTH]={0};
	int netaddr_bit = 0;
	
	if (ipaddr == NULL)
	{
		return (return_val);
	}

	
	iptype=check_ipaddr_type(ipaddr);
	//printf ("iptype:%d\n",iptype);
	if (iptype == 1)
	{
		//printf ("ipaddr:%s\n",ipaddr);
		
		inet_result=inet_aton(ipaddr, &inp);		//成功:非0；失败:0
		//printf ("inet_result:%d\n",inet_result);
		
		if (inet_result != 0)
			return_val = 0;
		else
			return_val = -1;
	}
	else if (iptype == 2)
	{
		strcpy(check_ipaddr, ipaddr);
		for (i=0,j=0,flag=0; i < strlen(check_ipaddr); i++){
			if (check_ipaddr[i] == '/'){
				flag = 1;
				continue;
			}
			if (flag == 0)
				ipaddr_head[i] = check_ipaddr[i];
			else{
				ipaddr_tail[j] = check_ipaddr[i];
				j++;
			}
		}
	
		if (check_ipaddr_dot(ipaddr_head, hostip_dot) != 3)
			return -1;
		
		inet_result=inet_aton(ipaddr_head, &inp);
		netaddr_bit=atoi(ipaddr_tail);
		if (inet_result !=0  &&  netaddr_bit > 0 &&  netaddr_bit <= 32)
			return_val = 0;
		else
			return_val = -1;
		
	}
	else if (iptype == 3)
	{
		strcpy(check_ipaddr, ipaddr);
		for (i=0,j=0,flag=0; i < strlen(check_ipaddr); i++){
			if (check_ipaddr[i] == '-'){
				flag = 1;
				continue;
			}
			if (flag == 0)
				ipaddr_head[i] = check_ipaddr[i];
			else{
				ipaddr_tail[j] = check_ipaddr[i];
				j++;
			}
		}
		
		if (check_ipaddr_dot(ipaddr_head, hostip_dot) != 3)
			return -1;
		if (check_ipaddr_dot(ipaddr_tail, hostip_dot) != 3)
			return -1;
		
		inet_result=inet_aton(ipaddr_head, &inp);
		if (inet_result != 0)
		{
			inet_result=inet_aton(ipaddr_tail, &inp2);
			if (inet_result != 0 && (ntohl(inp2.s_addr) > ntohl(inp.s_addr)))
				return_val = 0;
			else
				return_val = -1;
		}
		else
			return_val = -1;
	}
	else 
	{
		return_val = -1;
	}

	return (return_val);
}

//子网掩码合法性检查
//0 : 合法
//-1 : 不合法
int check_mask(char* subnet)  
{
    if(NULL == subnet)
        return -1;
    
    if(check_ipaddr(subnet) != 0)  
        return -1;

    unsigned int b = 0, i, n[4];  
    sscanf(subnet, "%u.%u.%u.%u", &n[3], &n[2], &n[1], &n[0]);  
    for(i = 0; i < 4; ++i) //将子网掩码存入32位无符号整型  
        b += n[i] << (i * 8);   
    b = ~b + 1;  
    if((b & (b - 1)) == 0)   //判断是否为2^n  
        return 0;  

    return -1; 
}


/*
int main(void)
{
	int result = 0;
	char ip[64]="192.168.0.d1";
	result=check_ipaddr(ip);
	printf("result:%d\n",result);
	return (0);
}*/
