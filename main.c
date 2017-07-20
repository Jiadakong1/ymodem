#include "Ymodem.h"

int main(int argc, char const *argv[]) {
    char buf[1029] = {0};
    printf("main:\n");
    uart_start();   //如果文件不初始化，

    //__putchar( 'C' );
    //__getbuf(buf, 1);
    fseek(pFile,0L,SEEK_SET);
    __getbuf(buf, 133);
    __putchar( ACK );
    __putchar( 'C' );
    // __getbuf(buf, 133);
    // __putchar( ACK );
    // __getbuf(buf, 133);
    // __putchar( NAK);
    // __getbuf(buf, 133);
    // __putchar( ACK );
    // __putchar( 'C' );
    // __getbuf(buf, 133);
    // __putchar( ACK );
    // ymodem_rx_put(buf,128);

    //ymodem_rx_put(buf, 1029);
    uart_end();

    return 0;
}
