#include "Ymodem.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern FILE *pFile;

void uart_start(){
  pFile = fopen ( "/dev/ttyPS0" , "r+") ;
}

void uart_end(){
    fclose(pFile);
    printf("uart close right!\n");
}



char __getchar(){
  char ch = '0';
 // fread(&ch, sizeof(char), sizeof(char) , pFile );
  fgetc(pFile);
  //printf("%c   ", ch);
  return ch;
}

void  __getbuf(char* buf, size_t len){
  //fread(buf, sizeof(char), len, pFile );
  //len = read(pFile,buf, len);
  len = fread(buf, sizeof(char), len , pFile );
  printf("getbuf: len=%d\n", len);
}


void __putchar( char ch ){
  char ch1 = ch;
  int len =0;
  len = fread(&ch1, sizeof(char), sizeof(char) , pFile );;

  printf("putchar: len = %d\n", len);
}


void __putbuf( char* buf, size_t len ){

}


// #include "Ymodem.h"
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
//
// extern int pFile;
//
//
//
// void uart_start(){
//   pFile = open ( "/dev/ttyPS0" , O_RDWR) ;
//
// }
//
// void uart_end(){
//     close(pFile);
//     printf("uart close right!\n");
//
// }
//
//
//
// char __getchar(){
//   char ch = '0';
//   read(pFile, &ch, sizeof(char));
//   flush;
//   sync;
//   //printf("%c   ", ch);
//   return ch;
// }
//
// void  __getbuf(char* buf, size_t len){
//   //fread(buf, sizeof(char), len, pFile );
//   //len = read(pFile,buf, len);
//   int i = 0;
//   for(i = 0; i < len; i++){
//       //printf("%d:", i);
//       buf[i] = __getchar();
//   }
//   //printf("getbuf: len=%d\n", len);
// }
//
//
// void __putchar( char ch ){
//   char ch1 = ch;
//   int len =0;
//   len = write(pFile, &ch1, sizeof(char));
//   //write(&ch1, sizeof(char), sizeof(char) , pFile );
//   printf("putchar: len = %d\n", len);
// }
//
//
// void __putbuf( char* buf, size_t len ){
//
// }
