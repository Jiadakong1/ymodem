#include "Ymodem.h"
#include <stddef.h>


//实现的接口函数
  //用户接口
static  FILE    *fdst;
static  size_t fil_rx_sz;
static  char   *fil_tx_nm;      //传输的文件名称
//static  size_t fil_tx_sz;       //传输的文件大小
  //接收
void  file_open(void)
{
    if( (fdst = fopen("receive.txt", "w") ) == NULL){
        printf("file open wrong!\n");
    }else{
        printf("file open right!\n");
    }
    return;
}

void file_close(void)
{
  if( fdst )
  {
    fclose( fdst );
    fdst = NULL;
  }
  return;
}



void write_buf_to_file(char *buf, size_t seek, size_t len){
    if(fdst){
        fseek(fdst, seek, SEEK_SET);
        fwrite(buf, sizeof(char), len, fdst);
    }
}









//   uint8 ymodem_rx_header(void)
//   {
//       if( (fdst = fopen("receive.txt", "r+") ) == NULL){
//           printf("file open wrong!\n");
//       }else{
//           printf("file open right!\n");
//       }
//
//       //fil_rx_sz = fil_sz;
//
//       return 0;
//
//
//     // uint8 ans = YMODEM_ERR;
//     // FRESULT res;
//     // cstr_t *px_cstr;
//     // if( fdst = (FIL*)pvPortMalloc(sizeof(FIL)) )
//     // {
//     //   if( px_cstr = cstr_create( "%s\\%s", px_path_cur->string, fil_nm ) )
//     //   {
//     //     res = f_open( fdst, px_cstr->string, FA_WRITE|FA_CREATE_ALWAYS );
//     //     cstr_free( px_cstr );
//     //     if( FR_OK == res )
//     //     {
//     //       fdst->fsize = fil_sz;
//     //       fil_rx_sz = fil_sz;
//     //       ans = YMODEM_OK;
//     //     }
//     //   }
//     // }
//     // if( YMODEM_ERR == ans )
//     // {vPortFree( fdst );fdst = NULL;}
//     // return ans;
//   }
// uint8 ymodem_rx_finish()
// {
//   if( fdst )
//   {
//     fclose( fdst );
//     fdst = NULL;
//   }
//   return YMODEM_OK;
// }
//
//
//
//
//
// uint8 ymodem_rx_pac_get( char *buf, size_t seek, size_t size )
// {
//   uint8 ans = YMODEM_ERR;
//   unsigned int wd;
//   if( fdst )
//   {
//     if( FR_OK == fseek(fdst, seek, SEEK_SET) )
//     {
//       if( fil_rx_sz < seek + size )
//         size = fil_rx_sz - seek;
//       if( FR_OK == fwrite( buf, sizeof(char), size, fdst) )
//       {
//         if( wd == size )
//           ans = YMODEM_OK;
//       }
//     }
//   }
//   return ans;
// }
