#ifndef _M_YMODEM_H
#define _M_YMODEM_H
/**************************************************************************************************
 *                                            INCLUDES
 **************************************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FALSE  -1
#define TRUE   0
#define FR_OK  0

/*********************************************************************
 * CONSTANTS
 */
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)     /* start, block, block-complement */
#define PACKET_TRAILER          (2)     /* CRC bytes */
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)
#define PACKET_TIMEOUT          (2000000)

#define INITIAL

#define FILE_NAME_LENGTH (64)
#define FILE_SIZE_LENGTH (16)

#define YMODEM_OK               0
#define YMODEM_ERR              1       //校验包是否有问题，只能是“YMODEM_ERR==”而不应该“YMODEM_OK!=”
#define YMODEM_PAC_EMPTY        2       //包校验正确，但是里面是空值，在（IDLE状态，判断是否需要结束，退出）
/* ASCII control codes: */
#define SOH (0x01)      /* start of 128-byte data packet */
#define STX (0x02)      /* start of 1024-byte data packet */
#define EOT (0x04)      /* end of transmission */
#define ACK (0x06)      /* receive OK */
#define NAK (0x15)      /* receiver error; retry */
#define CAN (0x18)      /* two of these in succession aborts transfer */
#define CNC (0x43)      /* character 'C' */

/* Number of consecutive receive errors before giving up: */
#define MAX_ERRORS    (5)

/*********************************************************************
 * TYPE_DEFS
 */
typedef unsigned char uint8;
int fd;
unsigned int time_out;
unsigned int time_count;
/*********************************************************************
 * FUNCTIONS
 *********************************************************************/
int packet_check(char *buf, int len);
int packet_if_empty( char *buf, int len);
void packet_processing(char *buf);
void packet_reception(char * buf);
void  file_open(void);
void file_close(void);
void write_buf_to_file(char *buf, size_t seek, size_t len);









//*注：接收——只有ymodem_rx_start()是接收到消息的时候调用，其它都是用户实现，ymodem自动调用
//*注：发送——用户调用ymodem_tx_header()只有ymodem_tx_start()是接收到消息的时候调用，其它都是用户实现，ymodem自动调用
void ymodem_rx_put( char *buf, size_t rx_sz );

//必须实现的函数
   //上层函数
   //接收函数
// uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz );          //当检测到包头的时候，自动调用此函数（打开文件）
// uint8 ymodem_rx_finish( uint8 status );                         //返回结束原因，成功还是出错（关闭文件）
// uint8 ymodem_rx_pac_get( char *buf, size_t offset, size_t size );//(写文件)

 //底层函数
void uart_start();
void uart_end();

char __getchar();
int __getbuf(char* buf, size_t len);

void __putchar( char ch );
void __putbuf( char* buf, size_t len );
#endif    //_M_YMODEM_H
