//lcm serial communication
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h> 

int fd= -1;
int showMessage=0;
int isExit=0;

const char *strType[]={"CPU LOAD","MODE", "MEM","VERSION","STATUS"};
const char *strTypeResult[]={"99.99%","NORMAL", "488/512M","3.0.8.1","READY"};

//set the serial option
int setOpt(int handle,int nSpeed, int nBits, char nEvent, int nStop)
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

void *getSerialData()
{
    int bytes=0;
    unsigned char cmd[2]={0};
    do
    {
        ioctl(fd, FIONREAD, &bytes);
        if (bytes>0)
        {
        	cmd[0]=0;
            read(fd, cmd, 1);
           	switch (cmd[0])
            {
            	case 0xE8:
            			//printf("dowm arraw\n\r");	
            			break;
            	case 0xB8:
            			//printf("UP arraw\n\r");	
            			break;
            	case 0xD8:
            	//	printf("enter key\n\r");	
            		isExit=1;
            			break;
            	case 0x78:
            		//printf("escape\n\r");	
            		showMessage=(showMessage+1)%5;
            			break;
            		default :
            			break;
            }
        }
        else
        {			
        	sleep(1);	
        }
    }while( 1 );	
}

int main(int argc, char **argv)
{
	char cmd[10]={0};
	int len=0;
	int pos =0;
	pthread_t thread_serial;
	
	//set the parameter format
	if ((argc<4) || (argc >4))
	{
			printf("the parameter format is ./command devicename first-line-pos data\n\r ");
			return -1;
	}
	//the first line beginning pos
	pos =atoi(argv[2]);
	
	fd= open(argv[1],O_RDWR |O_NONBLOCK);
	if(fd<0)
	{
		printf("serial open fail\n\r");	
		return -1;
	}
	
	setOpt(fd,9600,8,'N',1);
	
	pthread_create(&thread_serial,NULL,getSerialData,NULL);
	
	//clear screen cmd =0xf8 0x01
	printf("clear screen\n\r");
	cmd[0]=0xf8;
	cmd[1]=0x01;
	write(fd,cmd,2);
	
	len =strlen(argv[3]);
	//only display 16 in one line
	len=len>32?32:len;
	pos=pos>16?0:pos;
	/****************************************显示你希望显示数据******************************/
	//display first line begin position cmd = 0xf8 0x80 pos
	//printf("display begin position pos=%d\n\r ",pos);
	cmd[0]=0xf8;
	cmd[1]=0x80;
	cmd[2]=pos;
	write(fd,cmd,3);
	
	if ((len+pos)>16)
	{
		//display first line
		cmd[0]=0xf8;
		cmd[1]=0x03;
		cmd[2]=0xa0;
		write(fd,cmd,2);
		write(fd,argv[3],16-pos);	
		write(fd,&cmd[2],1);
		
		//the second line first address is 0x40
		cmd[0]=0xf8;
		cmd[1]=0x80;
		cmd[2]=0x40;
		write(fd,cmd,3);
		
		//display second line
		cmd[0]=0xf8;
		cmd[1]=0x03;
		cmd[2]=0xa0;
		write(fd,cmd,2);
		write(fd,argv[3]+16-pos,len-16+pos);
		write(fd,&cmd[2],1);
	}
	else
	{
			cmd[0]=0xf8;
			cmd[1]=0x03;
			cmd[2]=0xa0;
			write(fd,cmd,2);
			write(fd,argv[3],len);//
			write(fd,&cmd[2],1);
	}
	/****************************************end显示你希望显示数据******************************/
	/**********************************模式切换显示*********************************************/
	int currentMessage=0;
	while(!isExit)
	{
		if(currentMessage!=showMessage)
		{	
			currentMessage=showMessage;
			//清屏 f8 01
			cmd[0]=0xf8;
			cmd[1]=0x01;
			write(fd,cmd,2);
			
			//第一行显示位置 f8 80 pos
			cmd[0]=0xf8;
			cmd[1]=0x80;
			cmd[2]= (strlen(strType[currentMessage])>=16)?0:((16-strlen(strType[currentMessage]))/2);//显示从第三个位置开始
			write(fd,cmd,3);
			
			//第一行显示数据 f8 03 data a0
			cmd[0]=0xf8;
			cmd[1]=0x03;
			cmd[2]=0xa0;
			write(fd,cmd,2);
			write(fd,strType[currentMessage],strlen(strType[currentMessage]));//
			write(fd,&cmd[2],1);
			
			//第二行显示位置 f8 80 pos
			//the second line first address is 0x40
			cmd[0]=0xf8;
			cmd[1]=0x80;
			cmd[2]=(0x40+((strlen(strTypeResult[currentMessage])>=16)?0:((16-strlen(strTypeResult[currentMessage]))/2)));//显示从第三个位置开始
			write(fd,cmd,3);
		
			//第二行显示数据
			//display second line f8 03 data a0
			cmd[0]=0xf8;
			cmd[1]=0x03;
			cmd[2]=0xa0;
			write(fd,cmd,2);
			write(fd,strTypeResult[currentMessage],strlen(strTypeResult[currentMessage]));
			write(fd,&cmd[2],1);
		}
		else
		{
			sleep(1);	
		}
            
	}
	close(fd);
	
	return 0;	
}
