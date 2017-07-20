receive:main.o Ymodem.o uart.o Ymodem_port.o
	arm-linux-gnueabihf-gcc main.o Ymodem.o uart.o Ymodem_port.o -o receive -g

Ymodem_port.o:Ymodem_port.c  Ymodem.h
	arm-linux-gnueabihf-gcc -c Ymodem_port.c -g

Ymodem.o:Ymodem.c Ymodem.h Ymodem_port.o
	arm-linux-gnueabihf-gcc -c Ymodem.c -g

main.o:main.c  Ymodem.h Ymodem.o
	arm-linux-gnueabihf-gcc -c main.c -g

uart.o:uart.c  Ymodem.h
	arm-linux-gnueabihf-gcc -c uart.c -g




.PHONY:modules clean
clean:
	 rm *.o receive
