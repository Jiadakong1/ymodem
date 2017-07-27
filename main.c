#include "Ymodem.h"
#include <stdlib.h>     /*标准函数库定义*/

//
// #define PACKET_SIZE    128
// #define PACKET_1K_SIZE   1024

#define YMODEM_RX_IDLE          0
#define YMODEM_RX_ACK           1
#define YMODEM_RX_EOT           2
#define YMODEM_RX_ERR           3
#define YMODEM_RX_EXIT          4

/*********************************************************************
 * GLOBAL VARIABLES
 */
static  uint8 receive_status = YMODEM_RX_IDLE;
static  size_t packet_size;
static  size_t seek;
static  size_t packet_total_length;
static  int start_receive = TRUE;
static  int end_receive = FALSE;

//static size_t packet_type = PACKET_DATA;


int packet_check(char *buf, int len){
    char ch = buf[0];

    //指令包
    if(len <= 1){
        if( ch==EOT || ch==ACK || ch==NAK || ch==CAN || ch==CNC ){
            //printf("packet_check: %d\n", (int)ch );
            return (int)ch;

        }
        else
            return -1;
    }
    //数据包
    else{
        if( ch==SOH || ch==STX ){
            //数据包校验
            //printf("packet_check: %d\n", (int)ch );
            return (int)ch;
        }
        else
            return -1;      //错误的指令码
    }

}


int packet_if_empty( char *buf, int len)
{
    int offset=0;
    while( ((buf[offset]==0x20)||(buf[offset]==0x30) || (buf[offset]==0x00)) &&  ++offset<len);
    if( offset == len ){
        printf("是全0\n");
        return TRUE;
        printf("不是全0\n");
    }
    else
    return FALSE;
}



unsigned short crc16(const unsigned char *buf, unsigned long count)
{
  unsigned short crc = 0;
  int i;

  while(count--) {
    crc = crc ^ *buf++ << 8;

    for (i=0; i<8; i++) {
      if (crc & 0x8000) {
        crc = crc << 1 ^ 0x1021;
      } else {
        crc = crc << 1;
      }
    }
  }
  return crc;
}

void packet_processing(char *buf){
    int i = 0;
    unsigned short crc1 = 0;
    unsigned short crc2 = 0;
    //如果buf长度是0，那么发送字符c
    if( TRUE == start_receive){
        time_out = FALSE;   //开始新一次计时
        time_count = PACKET_TIMEOUT;
        __putchar('C');
        return;
    }
    //printf("start_receive = %d\n", (int)start_receive);
    printf("status = %d\n", (int)receive_status);
    //根据接收的状态进行处理
    switch (receive_status) {
        //接收第一包之前的状态
        case YMODEM_RX_IDLE:
            switch( packet_check( buf, packet_total_length) )   //检查当前包是否合法,并返回包的类型
            {
                case SOH:
                case STX:
                    printf("data packet:\t");
                    //packet_size = (size_t)(buf[0])==SOH ? PACKET_SIZE : PACKET_1K_SIZE;
                    if( TRUE == packet_if_empty( &buf[3], packet_size ) )   //判断是否是空包
                    {
                        __putchar( ACK );
                        printf("00000\n");

                        //__putchar( 0x4f);//xshell结束使用  crt不用
                        receive_status = YMODEM_RX_EXIT;
                        goto receive_exit;                  //这是在本循环必须完成的操作，所以需要用到 goto 语句
                    }
                    else    //如果不是空包，则认为是第一个包（包含文件名和文件大小）
                    {
                        printf("first packet!\n");
                        if( (packet_size==128)  || (packet_size==1024))//CRT第一个包为1024
                        {
                            __putchar( ACK );
                            seek = 0;      //初始化变量，用于接收新文件
                            __putchar( 'C' );
                            receive_status = YMODEM_RX_ACK;
                        }
                        else{
                          goto err; //在IDLE中接收到一个1024的数据包，则肯定是状态有问题
                        }
                    }
                    break;

                case EOT:
                  receive_status = YMODEM_RX_ERR;
                  goto err;                      //这是在本循环必须完成的操作，所以需要用到 goto 语句
                  break;
                default:
            //     __putchar( NAK );      //不正常的状态，调试用
                 goto err;              //这儿暂时认为，包有误，就退出
                 break;
            }
            break;


        //接收到第一个包后————>接收到EOT之前
        case YMODEM_RX_ACK:
            printf("%d\n", packet_total_length );
            switch (packet_check( buf, packet_total_length)) {
                case SOH:
                case STX:
                    //packet_size = (size_t)(buf[0])==SOH? PACKET_SIZE:PACKET_1K_SIZE;
                    crc1 = crc16( (unsigned char*)(buf+3), packet_size );
                    crc2 = ((unsigned short)(buf[packet_total_length-2]))*256+(unsigned short)buf[packet_total_length-1];
                    if( crc1 != crc2){
                        printf("crc wrong!\n");
                        exit(0);
                    }

                    write_buf_to_file( buf+3, seek, packet_size );  //将接收的包保存
                    seek += packet_size;
                    __putchar( ACK );
                    break;


                //指令包
                case EOT:
                    __putchar( NAK );
                    printf("YMODEM_RX_ACK: send NAK!\n" );
                    receive_status = YMODEM_RX_EOT;
                    break;
                case CAN:
                    receive_status = YMODEM_RX_ERR;
                    printf("YMODEM_RX_ACK: recieve CAN!\n" );
                    goto err;
                    break;
                default:
                  //__putchar( NAK );      //不正常的状态，调试用
                  receive_status = YMODEM_RX_ERR;
                  goto err;           //这儿暂时认为，包有误，就重发
                  printf("YMODEM_RX_ACK: default!\n" );
                  break;
            }
            break;


        //接收到第一个EOT之后————>接收到第二个EOT之前   状态变为YMODEM_RX_IDLE
        case YMODEM_RX_EOT:
            switch(packet_check( buf, packet_total_length))   //检查当前包是否合法,并返回包的类型
            {
                //指令包
                case EOT:
                    //ymodem_rx_finish( YMODEM_OK );        //确认发送完毕，保存文件
                    //start_receive = TRUE;
                    receive_status = YMODEM_RX_IDLE;
                    __putchar( ACK );
                    __putchar( 'C' );
                    printf("receive 2nd EOT:\n" );
                    break;

                default:
                    goto err;
                    break;
            }

            break;


        //在YMODEM_RX_IDLE状态下收到全0数据包
receive_exit:
        case YMODEM_RX_EXIT:
            printf("receive_exit\n");
            receive_status = YMODEM_RX_IDLE;
            end_receive = TRUE;
            return;

        //错误状态
err:
        case YMODEM_RX_ERR:
            printf("error!\n");
            receive_status = YMODEM_RX_IDLE;
            end_receive = TRUE;
            break;

        default:
            printf("wrong!\n");
            exit(1);
    }

}

void packet_reception(char * buf){
    int hdr_found = FALSE;
    int i = 0;

    //得到第一个字
    __getbuf(buf, 1);
    if(time_out == TRUE){   //超时要返回
        return;
    }


    switch (buf[0]) {
        case SOH:
        case STX:
          hdr_found = TRUE;
          break;

        case CAN:
            packet_size = 0;
            break;

        case EOT:
            packet_size = 0;
            break;

        default:
            break;
          /* Ignore, waiting for start of header */
    }
    //printf("\t\t\t type = %x\t\t", buf[0]);
    //得到剩下的字符
    if(hdr_found == TRUE){
        //packet_type = PACKET_DATA;
        packet_size = (size_t)(buf[0])==SOH ? PACKET_SIZE : PACKET_1K_SIZE;
        __getbuf(&buf[1], packet_size+4);
        packet_total_length = packet_size + 5;
    }
    else{
        //packet_type = PACKET_INSTRUCTION;
        packet_total_length = 1;
    }
    start_receive = FALSE;
}


int main(int argc, char const *argv[]) {
    char buf[1029] = {'0'};
    int i = 0;

    uart_start();   //如果文件不初始化，
    file_open();

    printf("start:\n");
    while(1) {
        packet_processing(buf);
        if(end_receive == TRUE)
            break;
        //printf("\n\n\n\nreceive data:\t");
        packet_reception(buf);
    }

    file_close();
    uart_end();
    return 0;
}
