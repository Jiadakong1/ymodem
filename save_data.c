#include "Ymodem.h"
#include <stddef.h>

static  FILE    *fp = NULL;

void  file_open(char *file_name)
{
    if( (fp = fopen(file_name, "w") ) == NULL)
    {
        printf("file open wrong!\n");
    }
    else
    {
        printf("file open right!\n");
        printf("file_name = %s\n",file_name );
        //while(1);
    }
    return;
}

void file_close(void)
{
  if( fp )
  {
    fclose( fp );
    fp = NULL;
  }
  return;
}


void write_buf_to_file(char *buf, size_t seek, size_t len)
{
    if(fp)
    {
        fseek(fp, seek, SEEK_SET);
        fwrite(buf, sizeof(char), len, fp);
    }
}


// void bin_file_0_to_255(void)     //将0-255以16进制写入文件，测试接收特殊字符用
// {
//     int i = 0;
//     if( (fp = fopen("bin_file_0_to_255.bin", "wb") ) == NULL)
//     {
//         printf("file open wrong!\n");
//     }
//     else
//     {
//         printf("file open right!\n");
//     }
//
//     for(i = 0; i<=255; i++)
//     {
//         fputc(i, fp);
//     }
//     fclose(fp);
//     printf("file close right!\n");
// }
