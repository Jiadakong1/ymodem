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
#include <stdlib.h>
#include <string.h>


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
#define PACKET_TIMEOUT          (800000)   //超时时间
#define MAX_TIMEOUT_NUM         (40)       //最大超时次数


#define INITIAL

#define FILE_NAME_LENGTH (64)
#define FILE_SIZE_LENGTH (16)


#define YMODEM_RX_IDLE          0
#define YMODEM_RX_ACK           1
#define YMODEM_RX_EOT           2
#define YMODEM_RX_ERR           3
#define YMODEM_RX_EXIT          4

/* ASCII control codes: */
#define SOH (0x01)      /* start of 128-byte data packet */
#define STX (0x02)      /* start of 1024-byte data packet */
#define EOT (0x04)      /* end of transmission */
#define ACK (0x06)      /* receive OK */
#define NAK (0x15)      /* receiver error; retry */
#define CAN (0x18)      /* two of these in succession aborts transfer */
#define CNC (0x43)      /* character 'C' */

typedef unsigned char uint8;

extern int fd;
extern unsigned int time_out;
extern unsigned int time_count;
extern unsigned int time_out_count;

extern uint8 receive_status;
extern size_t packet_size;
extern size_t seek;
extern size_t packet_total_length;
extern int file_name_len;
extern unsigned long file_size;
extern int start_receive;
extern int end_receive;


/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

extern void packet_processing(char *buf);
extern void packet_reception(char * buf);
extern void  file_open(char *file_name);
extern void file_close(void);
extern void write_buf_to_file(char *buf, size_t seek, size_t len);
//extern void bin_file_0_to_255(void);   //测试所有字符用

 //底层函数
extern void uart_start();
extern void uart_end();

extern int __getbuf(char* buf, size_t len);
extern void __putchar( char ch );

#endif    //_M_YMODEM_H
