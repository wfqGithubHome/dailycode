#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/io.h>

#include "lcmshow.h"

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format, args...)
#endif

void set_reuid(void)
{
	uid_t uid ,euid; 
	uid = getuid(); 
	euid = geteuid(); 
	setreuid(euid, uid);
}

void delay(int i)
{
    int j;
    for(j = 0; j < i; j++)
    {
        usleep(1000000);                 
    }    
}

int spliStr(char *strmsg, struct lcm_tare *rate)
{
    char uprate[STRING_LENTH] = {0};
    char downrate[STRING_LENTH] = {0};
    char time[STRING_LENTH]= {0};
    char iface[STRING_LENTH] = {0};
    char split_str[] = " ";
    char *result = NULL;
    int sign = 0;
    
    if(strmsg == NULL || rate == NULL){
        return -1;
    }

    result = strtok(strmsg,split_str);
    while(result != NULL){
        sign++;

        if(sign == 1){
            strncpy(iface,result,sizeof(iface)-1);
        }else if(sign == 2){
            strncpy(uprate,result,sizeof(uprate)-1);
        }else if(sign == 3){
            strncpy(downrate,result,sizeof(downrate)-1);
        }else if(sign == 4){
            strncpy(time,result,sizeof(time)-1);
        }else{
            break;
        }

        result = strtok(NULL,split_str);
    }

    strncpy(rate->iface, iface, STRING_LENTH-1);
    rate->tx_bytes = atof(uprate);
    rate->rx_bytes = atof(downrate);
    rate->time = atof(time);

    return 0;
}

//clear screen
void clc(int fd){
    char cmd[8] = {0};
    cmd[0]=0xf8;
	cmd[1]=0x01;

	write(fd,cmd,2);
    return;
}

static void do_lcm_temperature_show(unsigned char data, int fd)
{
    char showcmd[8] = {0};
    char *name = "cpu tempture:";
    char show_str[STRING_LENTH] = {0};
    int show_len;

    snprintf(show_str,sizeof(show_str),"%s%u",name,data);
    show_len = strlen(show_str)>16?16:strlen(show_str);

    clc(fd);

    //first line
    showcmd[0]=0xf8;
	showcmd[1]=0x80;
	showcmd[2]=0;
    write(fd,showcmd,3);

    showcmd[0]=0xf8;
	showcmd[1]=0x03;
	showcmd[2]=0xa0;
	write(fd,showcmd,2);
	write(fd,show_str,show_len);	
	write(fd,&showcmd[2],1); 

    delay(5);
    return;
}

static void do_lcm_rate_show(struct lcm_tare *rate1, struct lcm_tare *rate2, int fd)
{
    char showcmd[8] = {0};
    char *name[2] = {"UR:","DR:"};
    char *unit[3] = {"bps","Kbps","Mbps"};
    int tx_unit = 0;
    int rx_unit = 0;
    double tx_speed;        //发送包速度，下行
    double rx_speed;        //接收包速度，上行
    char tx_speed_str[STRING_LENTH] = {0};
    char rx_speed_str[STRING_LENTH] = {0};

    char first_line[STRING_LENTH] = {0};
    char second_line[STRING_LENTH] = {0};

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    if(rate1 == NULL || rate2 == NULL){
        return;
    }

    tx_speed = ((rate2->tx_bytes-rate1->tx_bytes)*8)/(rate2->time-rate1->time);
    rx_speed = ((rate2->rx_bytes-rate1->rx_bytes)*8)/(rate2->time-rate1->time);

    if(tx_speed > 1024 && tx_speed < 1024*1024){
        tx_speed = tx_speed/1024;
        tx_unit = 1;
    }else if(tx_speed > 1024*1024){
        tx_speed = tx_speed/(1024*1024);
        tx_unit = 2;
    }  

    if(rx_speed > 1024 && rx_speed < 1024*1024){
        rx_speed = rx_speed/1024;
        rx_unit = 1;
    }else if(rx_speed > 1024*1024){
        rx_speed = rx_speed/(1024*1024);
        rx_unit = 2;
    } 

    snprintf(tx_speed_str,sizeof(tx_speed_str),"%.2f%s",tx_speed,unit[tx_unit]);
    snprintf(rx_speed_str,sizeof(rx_speed_str),"%.2f%s",rx_speed,unit[rx_unit]);

    snprintf(first_line,sizeof(first_line),"%s%s",name[0],tx_speed_str);
    snprintf(second_line,sizeof(second_line),"%s%s",name[1],rx_speed_str);

    DEBUGP("%s: process, first_line:[%s], second_line:[%s]\n",__FUNCTION__,
            first_line,second_line);

    clc(fd);

    //first line
    showcmd[0]=0xf8;
	showcmd[1]=0x80;
	showcmd[2]=0;
    write(fd,showcmd,3);

    showcmd[0]=0xf8;
	showcmd[1]=0x03;
	showcmd[2]=0xa0;
	write(fd,showcmd,2);
	write(fd,first_line,strlen(first_line)>16?16:strlen(first_line));	
	write(fd,&showcmd[2],1); 

    //second line
    showcmd[0]=0xf8;
    showcmd[1]=0x80;
    showcmd[2]=0x40;
    write(fd,showcmd,3);
     
    showcmd[0]=0xf8;
    showcmd[1]=0x03;
    showcmd[2]=0xa0;
    write(fd,showcmd,2);
    write(fd,second_line,strlen(second_line)>16?16:strlen(second_line));
    write(fd,&showcmd[2],1);

    delay(4);
    DEBUGP("%s: finish\n", __FUNCTION__);
    return;
}

static void do_lcm_ip_show(const char *ip, int fd)
{
    char name[8] = "LAN IP";
    char showcmd[STRING_LENTH] = {0};

    clc(fd);

    //first line
    showcmd[0]=0xf8;
	showcmd[1]=0x80;
	showcmd[2]=5;
    write(fd,showcmd,3);

    showcmd[0]=0xf8;
	showcmd[1]=0x03;
	showcmd[2]=0xa0;
	write(fd,showcmd,2);
	write(fd,name,strlen(name));	
	write(fd,&showcmd[2],1); 

    //second line
    showcmd[0]=0xf8;
    showcmd[1]=0x80;
    showcmd[2]=0x41;
    write(fd,showcmd,3);
     
    showcmd[0]=0xf8;
    showcmd[1]=0x03;
    showcmd[2]=0xa0;
    write(fd,showcmd,2);
    write(fd,ip,strlen(ip)>16?16:strlen(ip));
    write(fd,&showcmd[2],1);

    delay(5);
    return;
}


static int do_get_rate(char *str, int maxlen)
{
    char strmsg[SHOW_LENGTH] = {0};
    char *cmd = "cat /proc/net/dev|sed 's/:/ /g'| awk 'BEGIN{\"cat /proc/uptime\"|getline time;} { if($1~/INT0$/) print $1,$2,$10,time;}'";
    FILE *pp = NULL;

    if(str == NULL){
        return -1;
    }

    set_reuid();
    pp = popen(cmd,"r");
    if(NULL == pp){
        return -1;
    }
    fgets(strmsg,sizeof(strmsg)-1,pp);
    pclose(pp);
    if(strlen(strmsg) == 0){
        return -1;
    }
    strmsg[strlen(strmsg)-1] = 0;

    strncpy(str,strmsg,maxlen-1);
    return 0;
}

//show rate
int show_rate(int fd, int ret)
{
    char strmsg[SHOW_LENGTH] = {0};
    struct lcm_tare rate1;
    struct lcm_tare rate2;
    struct lcm_rate_show result;

    if(ret == 0){
        return 0;
    }

    DEBUGP("%s: begin\n",__FUNCTION__);

    if(do_get_rate(strmsg,sizeof(strmsg)) != 0){
        return -1;
    }
    
    DEBUGP("%s:process, msg:[%s]\n", __FUNCTION__, strmsg);
    
    memset(&rate1,0,sizeof(rate1));
    if(spliStr(strmsg,&rate1) != 0){
        return -1;
    }

    DEBUGP("%s:process, iface:%s, up:%.2f, down:%.2f, time:%.2f\n",__FUNCTION__,
            rate1.iface,rate1.rx_bytes,rate1.tx_bytes,rate1.time);

    //delay one second
    delay(1);

    memset(strmsg,0,sizeof(strmsg));
    if(do_get_rate(strmsg,sizeof(strmsg)) != 0){
        return -1;
    }
    
    DEBUGP("%s:process, msg:[%s]\n", __FUNCTION__, strmsg);
    
    memset(&rate2,0,sizeof(rate2));
    if(spliStr(strmsg,&rate2) != 0){
        return -1;
    }

    DEBUGP("%s:process, iface:%s, up:%.2f, down:%.2f, time:%.2f\n",__FUNCTION__,
            rate2.iface,rate2.rx_bytes,rate2.tx_bytes,rate2.time);

    memset(&result,0,sizeof(result));
    do_lcm_rate_show(&rate1,&rate2,fd);

    DEBUGP("%s: finish\n", __FUNCTION__);
    return 0;
}

int show_temperature(int fd, int ret)
{
    uint8_t value;
    //uint32_t temp; 

    if(ret == 0){
        return 0;
    }

    if(iopl(3) == -1){
        printf("errno:%d\n", errno);
        return -1;
    }
    
    //temp =(uint32_t)value; 
#if 1    
    //选择温度寄存器所在的bank1
    outb(0x4E,0x295);
    outb(0x01,0x296);
    usleep(10000);

    outb(0x50,0x295);
    value = inb(0x296);

    value = value - 27;     //感温器件误差，需减掉27

    if(value  > 90)
    {
        outb(0x29,0xa15); 
        value = inb(0xa16);
    }
#endif
    DEBUGP("%s: process, temp:%u\n", __FUNCTION__, temp);

    iopl(0);

    do_lcm_temperature_show(value,fd);
    return 0;
}

//show ip
int show_ip(int fd, int ret)
{
    char lanIp[STRING_LENTH] = {0};
    char *cmd = "/usr/local/sbin/ip addr show | grep \"scope global LAN\" |awk '{print $2}' |awk -F '/' '{print $1}'";
    FILE *pp = NULL;

    if(ret == 0){
        return 0;
    }

    DEBUGP("%s:begin\n",__FUNCTION__);
    
    set_reuid();
    pp = popen(cmd,"r");
    if(NULL == pp){
        return -1;
    }

    fgets(lanIp,sizeof(lanIp)-1,pp);
    lanIp[strlen(lanIp)-1] = 0;
    pclose(pp);

    if(strlen(lanIp) == 0){
        return -1;
    }
    DEBUGP("%s:process, lanip:[%s]\n",__FUNCTION__,lanIp);

    do_lcm_ip_show(lanIp,fd);
    DEBUGP("%s:finish\n",__FUNCTION__);
    return 0;
}

//set the serial option
int set_opt_str(int handle,int nSpeed, int nBits, char nEvent, int nStop)
{
		struct termios newtio;
		cfmakeraw(&newtio);
		
		switch( nBits )
		{
				case 7:
				{
				    newtio.c_cflag |= CS7;      //设置 字符长度掩码
						break;
				}
				case 8:
				{
				    newtio.c_cflag |= CS8;
				    break;
				}
		}

		switch( nEvent )
		{
				case 'o':
				case 'O':
				{
				    newtio.c_cflag |= PARENB;
				    newtio.c_cflag |= PARODD;
				    break;
				}
				case 'e'	:
				case 'E':
				{
				   newtio.c_cflag |= PARENB;
				   newtio.c_cflag &= ~PARODD;
				   break;
				}
				case 'n':
				case 'N':
		        {
		            newtio.c_cflag &= ~PARENB;
		            break;
		        }
		}

		switch( nSpeed )
		{
			case 1200:
			{
			        cfsetispeed(&newtio, B1200);    //设置输入波特率
			        cfsetospeed(&newtio, B1200);    //设置输出波特率
			        break;
			}
			case 1800:
			{
			        cfsetispeed(&newtio, B1800);
			        cfsetospeed(&newtio, B1800);
			        break;
			}
			case 2400:
			{
			        cfsetispeed(&newtio, B2400);
			        cfsetospeed(&newtio, B2400);
			        break;
			}
			case 4800:
			{
			        cfsetispeed(&newtio, B4800);
			        cfsetospeed(&newtio, B4800);
			        break;
			}
			case 9600:
			{
			        cfsetispeed(&newtio, B9600);
			        cfsetospeed(&newtio, B9600);
			        break;
			}
			case 19200:
			{
			        cfsetispeed(&newtio, B19200);
			        cfsetospeed(&newtio, B19200);
			        break;
			}
			case 38400:
			{
			        cfsetispeed(&newtio, B38400);
			        cfsetospeed(&newtio, B38400);
			        break;
			}
			case 57600 :
			{
			        cfsetispeed(&newtio, B57600);
			        cfsetospeed(&newtio, B57600);
			        break;
			}
			case 115200:
			{
			        cfsetispeed(&newtio, B115200);
			        cfsetospeed(&newtio, B115200);
			        break;
			}
			case 230400 :
			{
			        cfsetispeed(&newtio, B230400);
			        cfsetospeed(&newtio, B230400 );
			        break;
			}
			case 460800:
			{
			        cfsetispeed(&newtio, B460800);
			        cfsetospeed(&newtio, B460800);
			        break;
			}
			default:
			{
			        cfsetispeed(&newtio, B9600);
			        cfsetospeed(&newtio, B9600);
			        break;
			}
		}

		if( nStop == 1 )
		{
		        newtio.c_cflag &=  ~CSTOPB;
		}
		else if ( nStop == 2 )
		{
		        newtio.c_cflag |=  CSTOPB;
		}
		
		newtio.c_cflag |= CLOCAL|CREAD;
		
		if((tcsetattr(handle, TCSANOW, &newtio)) != 0)   //设置与终端相关的参数,  TCSANOW:改变立即发生
		{
		        return -1;
		}
		return 0;
}


int main(void)
{
    int fd;

    fd = open("/dev/ttyS1",O_RDWR |O_NONBLOCK);
    if(fd < 0){
        exit(1);
    }
    set_opt_str(fd,9600,8,'N',1);

    while(1){
        show_ip(fd,1);
        show_rate(fd,1);
        show_temperature(fd,1);
    }
    close(fd);
    
    return 0;
}
