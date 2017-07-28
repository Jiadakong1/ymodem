receive:main.o Ymodem.o uart.o save_data.o
	arm-linux-gnueabihf-gcc main.o Ymodem.o uart.o save_data.o -o receive -g

save_data.o:save_data.c  Ymodem.h
	arm-linux-gnueabihf-gcc -c save_data.c -g

Ymodem.o:Ymodem.c Ymodem.h save_data.o
	arm-linux-gnueabihf-gcc -c Ymodem.c -g

main.o:main.c  Ymodem.h Ymodem.o
	arm-linux-gnueabihf-gcc -c main.c -g

uart.o:uart.c  Ymodem.h
	arm-linux-gnueabihf-gcc -c uart.c -g




.PHONY:modules clean
clean:
	 rm *.o receive
