#include "Ymodem.h"

/* GLOBAL VARIABLES*/
uint8 receive_status = YMODEM_RX_IDLE;
size_t packet_size = 0;
size_t seek = 0;
size_t packet_total_length = 0;
int file_name_len = 0;
unsigned long file_size = 0;
int start_receive = TRUE;
int end_receive = FALSE;


static int packet_check(char *buf, int len)
{
    char ch = buf[0];

    //指令包
    if(len <= 1)
    {
        if( ch==EOT || ch==ACK || ch==NAK || ch==CAN || ch==CNC )
        {
            return (int)ch;
        }
        else
            return -1;
    }
    //数据包
    else
    {
        if( ch==SOH || ch==STX )
        {
            return (int)ch;
        }
        else
            return -1;
    }
}

static int packet_if_empty( char *buf, int len)
{
    int offset=0;
    while( ((buf[offset]==0x20)||(buf[offset]==0x30) || (buf[offset]==0x00)) &&  ++offset<len); //CRT结尾并不都是0x1a，注意
    if( offset == len )
    {
        printf("0 packet!\n");
        return TRUE;
        printf("not 0 packet!\n");
    }
    else
    return FALSE;
}


static unsigned short crc16(const unsigned char *buf, unsigned long count)
{
    unsigned short crc = 0;
    int i;

    while(count--)
    {
        crc = crc ^ *buf++ << 8;
        for (i=0; i<8; i++)
        {
            if(crc & 0x8000)
            {
                crc = crc << 1 ^ 0x1021;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

unsigned long str_to_u32(const char* str)
{
    const char *s = str;
    unsigned long len;
    int c;

    do {                //得到第一个字符，跳过空格
        c = *s++;
    } while (c == ' ');

    for (len = 0; (c >= '0') && (c <= '9'); c = *s++) {
        c -= '0';
        len *= 10;
        len += c;
    }
    return len;
}


void packet_processing(char *buf){
    int i = 0;
    unsigned short crc1 = 0;
    unsigned short crc2 = 0;
    //开始接收，发送字符c
    if( TRUE == start_receive){                 //packet_reception函数接收到数据之后将start_receive置为FALSE
        time_out = FALSE;                       //开始新一次计时
        time_count = PACKET_TIMEOUT;
        __putchar('C');
        return;
    }
    //printf("status = %d\n", (int)receive_status);
    switch (receive_status)
    {
        case YMODEM_RX_IDLE:                                    //接收第一包之前的状态
            switch( packet_check( buf, packet_total_length) )   //检查当前包是否合法,并返回包的类型
            {
                case SOH:
                case STX:
                    if( TRUE == packet_if_empty( buf+3, packet_size ) )   //判断是否是空包
                    {
                        __putchar( ACK );
                        //__putchar( 0x4f);//xshell结束使用  crt不用
                        receive_status = YMODEM_RX_EXIT;
                        goto receive_exit;                      //这是在本循环必须完成的操作，所以需要用到 goto 语句
                    }
                    else                                        //如果不是空包，则认为是第一个包（包含文件名和文件大小）
                    {
                        printf("first packet!\n");
                        __putchar( ACK );
                        seek = 0;                               //初始化变量，用于接收新文件
                        __putchar( 'C' );
                        file_open(buf+3);                     //解析文件名
                        file_name_len = strlen(buf+3);
                        file_size = (size_t)str_to_u32( buf+3+file_name_len+1 ); //解析文件长度
                        //printf("file_name_len = %d\t file_size = %d", file_name_len, (int)file_size);
                        receive_status = YMODEM_RX_ACK;
                    }
                    break;

                case EOT:
                    receive_status = YMODEM_RX_ERR;
                    goto err;
                    break;

                default:
                    goto err;
                    break;
            }
            break;

        case YMODEM_RX_ACK:                                      //接收到第一个包后————>接收到EOT之前
            switch (packet_check( buf, packet_total_length))
            {
                case SOH:  //数据包
                case STX:
                    //packet_size = (size_t)(buf[0])==SOH? PACKET_SIZE:PACKET_1K_SIZE;
                    crc1 = crc16( (unsigned char*)(buf+3), packet_size );
                    crc2 = ((unsigned short)(buf[packet_total_length-2]))*256+(unsigned short)buf[packet_total_length-1];
                    if( crc1 != crc2)
                    {
                        printf("crc wrong!\n");
                        exit(0);
                    }

                    if( file_size < seek + packet_size )      //最后一包处理
                        packet_size = file_size - seek;

                    write_buf_to_file( buf+3, seek, packet_size );  //将接收的包保存
                    seek += packet_size;
                    __putchar( ACK );
                    break;

                case EOT:  //指令包
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
                    receive_status = YMODEM_RX_ERR;
                    goto err;
                    printf("YMODEM_RX_ACK: default!\n" );
                    break;
            }
            break;

        case YMODEM_RX_EOT:                                      //接收到第一个EOT之后————>接收到第二个EOT之前   状态变为YMODEM_RX_IDLE
            switch(packet_check( buf, packet_total_length))      //检查当前包是否合法,并返回包的类型
            {
                //指令包
                case EOT:
                    //printf("receive 2nd EOT:\n" );
                    receive_status = YMODEM_RX_IDLE;
                    file_close();                               //文件接收完毕，关闭文件
                    __putchar( ACK );
                    __putchar( 'C' );
                    break;
                default:
                    goto err;
                    break;
            }
            break;

receive_exit:
        case YMODEM_RX_EXIT:                                    //在YMODEM_RX_IDLE状态下收到全0数据包
            printf("receive_exit: YMODEM_RX_EXIT\n");
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


void packet_reception(char * buf)
{
    int hdr_found = FALSE;
    int i = 0;

    //得到第一个字
    __getbuf(buf, 1);
    if(time_out == TRUE)   //超时要返回
    {
        return;
    }
    switch (buf[0])
    {
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
    }

    //得到剩下的字符
    if(hdr_found == TRUE)
    {
        packet_size = (size_t)(buf[0])==SOH ? PACKET_SIZE : PACKET_1K_SIZE;
        __getbuf(buf+1, packet_size+4);
        packet_total_length = packet_size + 5;
    }
    else
    {
        packet_total_length = 1;
    }
    //printf("packet_total_length = %d\n", packet_total_length );

    start_receive = FALSE;
}
