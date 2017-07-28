#include "Ymodem.h"

int main(int argc, char const *argv[])
{
    char buf[1029] = {'0'};
    int i = 0;
    uart_start();   //如果文件不初始化，

    printf("start:\n");
    while(1)
    {
        packet_processing(buf);
        if(end_receive == TRUE)
            break;
        //printf("\n\n\n\nreceive data:\t");
        packet_reception(buf);
    }

    uart_end();
    return 0;
}
