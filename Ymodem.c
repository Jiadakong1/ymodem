// /**************************************************************************************************
//  *                                            INCLUDES
//  **************************************************************************************************/
// #include "Ymodem.h"
// #include <stdio.h>
// #include <string.h>
//
// /*********************************************************************
//  * CONSTANTS
//  */
// #define YMODEM_DATA_SIZE_128    128
// #define YMODEM_DATA_SIZE_1024   1024
//
// #define YMODEM_RX_IDLE          0
// #define YMODEM_RX_ACK           1
// #define YMODEM_RX_EOT           2
// #define YMODEM_RX_ERR           3
// #define YMODEM_RX_EXIT          4
//
// #define YMODEM_TX_IDLE          0
// #define YMODEM_TX_IDLE_ACK      1
// #define YMODEM_TX_DATA          2
// #define YMODEM_TX_DATA_ACK      3
// #define YMODEM_TX_EOT           4
// #define YMODEM_TX_ERR           5
// #define YMODEM_TX_EXIT          6
// /*********************************************************************
//  * GLOBAL VARIABLES
//  */
// static  uint8 ym_rx_status = YMODEM_RX_IDLE;
// static  uint8 ym_tx_status = YMODEM_RX_IDLE;
// static  size_t pac_size;
// static  size_t seek;
//
// static  uint8 ym_cyc;   //发送时的轮转变量
// /*********************************************************************
//  * EXTERNAL VARIABLES
//  */
//
// /*********************************************************************
//  * EXTERNAL FUNCTIONS
//  *********************************************************************/
//
// /*********************************************************************
//  * TYPE_DEFS
//  */
//
// /*********************************************************************
//  * FUNCTIONS
//  *********************************************************************/
// uint8 ymodem_rx_header( char* fil_nm, size_t fil_sz );
// uint8 ymodem_rx_finish( uint8 status );
// uint8 ymodem_rx_pac_get( char *buf, size_t seek, size_t size );
//
// void __putchar( char ch );
// void __putbuf( char* buf, size_t len );
//
// //核心函数
// unsigned short crc16(const unsigned char *buf, unsigned long count)
// {
//   unsigned short crc = 0;
//   int i;
//
//   while(count--) {
//     crc = crc ^ *buf++ << 8;
//
//     for (i=0; i<8; i++) {
//       if (crc & 0x8000) {
//         crc = crc << 1 ^ 0x1021;
//       } else {
//         crc = crc << 1;
//       }
//     }
//   }
//   return crc;
// }
//
// static const char *u32_to_str(unsigned int val)
// {
//   /* Maximum number of decimal digits in u32 is 10 */
//   static char num_str[11];
//   int  pos = 10;
//   num_str[10] = 0;
//
//   if (val == 0) {
//     /* If already zero then just return zero */
//     return "0";
//   }
//
//   while ((val != 0) && (pos > 0)) {
//     num_str[--pos] = (val % 10) + '0';
//     val /= 10;
//   }
// return &num_str[pos];
// }
//
// static unsigned long str_to_u32(char* str)
// {
//   const char *s = str;
//   unsigned long acc;
//   int c;
//
//   /* strip leading spaces if any */
//   do {
//     c = *s++;
//   } while (c == ' ');
//
//   for (acc = 0; (c >= '0') && (c <= '9'); c = *s++) {
//     c -= '0';
//     acc *= 10;
//     acc += c;
//   }
//   return acc;
// }
// //返回包的类型
// uint8 ymodem_rx_pac_check( char* buf, size_t sz )
// {
//   char ch;
//   ch = buf[0];
//   if( sz < 128 ) //是个指令包
//   {
//     if( ch==EOT || ch==ACK || ch==NAK || ch==CAN || ch==CNC )
//     {
//       int i=1;
//       while( i<sz && buf[i++]==ch );    //判断包中所有内容是否一样
//       if( sz == i )     //是全部一样的话，则认为此命令包有效
//         return ch;
//       else
//         return 0xff;
//     }
//     else
//       return 0xff;      //错误的指令码
//   }
//   else
//   {
//     if( ch==SOH || ch==STX )
//     {
//     unsigned short crc1 = crc16( (size_t*)(buf+PACKET_HEADER), sz-PACKET_OVERHEAD );
//     unsigned short crc2 = ((unsigned short)(buf[sz-2]))*256+buf[sz-1];
//     if( crc1 == crc2 && 0xff == (size_t)buf[1]+(size_t)buf[2] )
//       return ch;
//     else
//       return 0xff;      //数据包校验为错
//     }
//     else
//       return 0xff;      //错误的指令码
//   }
// }
// //**********************************************************************接收部分
// uint8 ymodem_rx_pac_if_empty( char *buf, size_t sz )
// {
//   size_t offset=0;
//   while( buf[offset]==0x00 && ++offset<sz );
//   if( offset==sz )
//     return TRUE;
//   else
//     return FALSE;
// }
// uint8 ymodem_rx_prepare( char *buf, size_t sz ) //解析出头包中的文件名和大小
// {
//   size_t ans = YMODEM_OK;
//   char *fil_nm;
//   size_t   fil_nm_len;
//   size_t fil_sz;
//   fil_nm = buf;
//   fil_nm_len = strlen( fil_nm );
//   fil_sz = (size_t)str_to_u32( buf+fil_nm_len+1 );
//   ans = ymodem_rx_header( fil_nm, fil_sz );
//   return ans;
// }
// /*********************************************************************
//  * @fn      ymodem_tx_put : Ymodem接收时，逻辑轮转调用函数
//  * @param   buf : 数据缓冲区 buf : 数据大小
//  */
// extern void ymodem_rx_put( char *buf, size_t rx_sz )
// {
//   if( 0 == rx_sz )      //超时，从而得到的长度为0，则尝试发送“C”，并返回
//   {__putchar( 'C' );return;}
//
//   switch( ym_rx_status )
//   {
//   case YMODEM_RX_IDLE:
//     switch( ymodem_rx_pac_check( buf, rx_sz ) )   //检查当前包是否合法,并返回包的类型
//     {
//     case SOH:
//     case STX:
//       pac_size = (size_t)(buf[0])==SOH? PACKET_SIZE:PACKET_1K_SIZE;
//       if( TRUE == ymodem_rx_pac_if_empty( buf+PACKET_HEADER, pac_size ) )   //判断是否是空包
//       {
//         __putchar( ACK );
//         ym_rx_status = YMODEM_RX_EXIT;
//         goto exit;                  //这是在本循环必须完成的操作，所以需要用到 goto 语句
//       }
//       else    //如果不是空包，则认为是第一个包（包含文件名和文件大小）
//       {
//         if( pac_size==128 && YMODEM_OK == ymodem_rx_prepare( buf+PACKET_HEADER, pac_size ) )
//         {
//           __putchar( ACK );
//           seek = 0;      //初始化变量，用于接收新文件
//           __putchar( 'C' );
//           ym_rx_status = YMODEM_RX_ACK;
//         }
//         else
//           goto err; //在IDLE中接收到一个1024的数据包，则肯定是状态有问题
//       }
//       break;
//     case EOT:
//       ym_rx_status = YMODEM_RX_EXIT;
//       goto exit;                      //这是在本循环必须完成的操作，所以需要用到 goto 语句
//       break;
//     default:
// //     __putchar( NAK );      //不正常的状态，调试用
//      goto err;              //这儿暂时认为，包有误，就退出
//      break;
//     }
//     break;
//     case YMODEM_RX_ACK:                                         //1级——文件接收状态中
//       switch( ymodem_rx_pac_check( buf, rx_sz ) )   //检查当前包是否合法,并返回包的类型
//       {
//         case SOH:
//         case STX:
//           __putchar( ACK );
//           pac_size = (size_t)(buf[0])==SOH? PACKET_SIZE:PACKET_1K_SIZE;
//           ymodem_rx_pac_get( buf+PACKET_HEADER, seek, pac_size );  //将接收的包保存
//           seek += pac_size;
//          //???? __putchar( 'C' );
//           break;
//           //指令包
//         case EOT:
//           __putchar( NAK );
//           ym_rx_status = YMODEM_RX_EOT;
//           break;
//         case CAN:
//           ym_rx_status = YMODEM_RX_ERR;
//           goto err;
//           break;
//         default:
//           __putchar( NAK );      //不正常的状态，调试用
// //          goto err;           //这儿暂时认为，包有误，就重发
//           break;
//       }
//       break;
//     case YMODEM_RX_EOT:         //在这里保存文件
//     {
//       switch( ymodem_rx_pac_check( buf, rx_sz ) )   //检查当前包是否合法,并返回包的类型
//       {
//           //指令包
//         case EOT:
//           __putchar( ACK );
//           ymodem_rx_finish( YMODEM_OK );        //确认发送完毕，保存文件
//           ym_rx_status = YMODEM_RX_IDLE;
//           break;
//         default:
//           goto err;
//           break;
//       }
//     }
//       break;
// err:  case YMODEM_RX_ERR:         //在这里放弃保存文件,终止传输
//       printf("1:\n");
//       __putchar( CAN );
//       printf("2:\n");
//       ymodem_rx_finish( YMODEM_ERR );
//       //break;                    //没有break，和下面公用代码
// exit: case YMODEM_RX_EXIT:        //到这里，就收拾好，然后退出
//       ym_rx_status = YMODEM_RX_IDLE;
//       //*这里还需要进行某些操作，使在退出后，不会再重新进入ymodem_rx_put()函数
//       //usart_protocol_model_cur = USART_PROTOCOL_DEFAULT;
//       return;
//     default:
//       break;
//   }
// }
//
// void  file_open(char *file_name)
// {
//     if( (fdst = fopen(file_name, "w") ) == NULL){
//         printf("file open wrong!\n");
//     }else{
//         printf("file open right!\n");
//         printf("file_name = %s\n",file_name );
//         //while(1);
//     }
//     return;
// }
