#include "Ymodem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/

int fd = 0;
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = { B460800,B230400,B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = { 460800, 230400,115200, 38400,  19200,   9600,  4800,  2400,  1200,  300, };

unsigned int time_out = FALSE;
unsigned int time_count = PACKET_TIMEOUT;
unsigned int time_out_count = 0;

void set_speed(int fd, int speed)
{
       int   i;
       int   status;
       struct termios   Opt;
       tcgetattr(fd, &Opt);  //得到设备对应的termios，保存到opt,修改opt，再写回。
       for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
       {
             if  (speed == name_arr[i])
             {
                  tcflush(fd, TCIOFLUSH);
                  cfsetispeed(&Opt, speed_arr[i]);  //改变opt中表示输入速度元素的值
                  cfsetospeed(&Opt, speed_arr[i]);
                  status = tcsetattr(fd, TCSANOW, &Opt);  //根据opt设置设备termios值
                  if  (status != 0)
                  {
                           perror("tcsetattr fd");
                           return;
                  }
                  tcflush(fd,TCIOFLUSH);
             }
       }
}
  /**
  *@brief   设置串口数据位，停止位和效验位
  *@param  fd     类型  int  打开的串口文件句柄
  *@param  databits 类型  int 数据位   取值 为 7 或者8
  *@param  stopbits 类型  int 停止位   取值为 1 或者2
  *@param  parity  类型  int  效验类型 取值为N,E,O,,S
  */
int set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
         perror("SetupSerial 1");
         return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*///设置为普通模式，当串口作为中断时，设置为标准模式
    options.c_oflag  &= ~OPOST;   /*Output*/

    //options.c_iflag &= ~ (IXON | IXOFF| BRKINT | ISTRIP  | IXANY | ICRNL | IGNCR);//自己加的，屏蔽软件流控  如果不屏蔽，0x11接收不到
    options.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    switch (databits)    /*设置数据位数*/
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unsupported data size/n"); return (FALSE);
    }

    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;
        default:
            fprintf(stderr,"Unsupported parity/n");
            return (FALSE);
    }
    /* 设置停止位*/
    switch (stopbits)
    {
        case 1:
             options.c_cflag &= ~CSTOPB;
             break;
        case 2:
             options.c_cflag |= CSTOPB;
             break;
        default:
              fprintf(stderr,"Unsupported stop bits/n");
              return (FALSE);
    }
  /* Set input parity option */
    if ((parity != 'n') && (parity != 'N'))
        options.c_iflag |= INPCK;
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 0; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0;//13; /* define the minimum bytes data to be readed*/

    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
        return (TRUE);
}
  /**********************************************************************
  代码说明：使用串口一测试的，发送的数据是字符，
  但是没有发送字符串结束符号，所以接收到后，后面加上了结束符号
  **********************************************************************/

  /*********************************************************************/

void uart_start()
{
    fd = open( "/dev/ttyS1", O_RDWR | O_NOCTTY | O_NDELAY); //注意：红板是/dev/ttyPS1  蓝板是/dev/ttyS1

    if (-1 == fd)
    {
        perror("Can't Open Serial Port");
    }
    else
    {
        set_speed(fd, 115200);  //设置为115200有问题，不设置也有问题
        if (set_Parity(fd,8,1,'N') == FALSE)
        {
            printf("Set Parity Error/n");
        }
    }

}

void uart_end()
{
    fcntl(fd,F_SETFL,0);//阻塞
    close(fd);
    printf("uart close right!\n");
}

void __putchar( char ch )
{
  char ch1 = ch;
  int len =0;
  while( write(fd, &ch1, sizeof(char)) <= 0)
  {
      printf("write wrong!\n");
  }
}

int   __getbuf(char* buf, size_t len)
{
    int nread = 0;
    int be_left = len;
    int i = 0;
    //printf("\n__getbuf:  data\n");
    while(1)
    {
        //usleep(5000);
        nread = read(fd, &buf[len-be_left], be_left);
        if(nread > 0)
        {
            time_count = PACKET_TIMEOUT;//设置超时时间
            // for(i=len-be_left; i<len-be_left+nread; i++)//打印已经接收的数据
            // {
            //     printf("%x ", buf[i]);     //%x以16进制显示
            // }
            be_left = be_left - nread;
        }
        else
        {
            //超时
            time_count--;
            if(time_count <= 0)
            {
                time_out = TRUE;
                printf("time out !      len = %d\t be_left = %d\n", len, be_left);
                time_out_count++;
                if(time_out_count >= MAX_TIMEOUT_NUM)
                    exit(1);
                break;
            }
        }
        if(be_left == 0)
           break;
    }
    printf("\n");
    return len - be_left;
}
