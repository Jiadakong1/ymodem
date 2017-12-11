


## 板卡配置
- 根据串口设备文件
修改uart.c文件
uart_start()函数
fd = open( "/dev/<font color=#ff0000 size=4s>ttyS?</font>", O_RDWR | O_NOCTTY | O_NDELAY);
- 如果配置错误，程序会报错： write wrong！

<!--![深鉴科技](http://www.deephi.com/img/teamLeader.jpg)-->

## 程序参数:
- 使用程序时可以加参数，目前只支持波特率的设置： 如 ./receive 9600     默认为115200

## 主要文件的作用:

+ uart.c :  串口操作函数

* save_data.c : 存储数据用，暂时将数据存入文件中

* Ymodem.c ： ymodem协议中用到的各种函数


## 注意
* CRT结尾包并不都是0x1a，为了适应传CRT  ((buf[offset]==0x20)||(buf[offset]==0x30) || (buf[offset]==0x00)  
问题：如果全是空格也会当成结束的  
这个问题不会出现： 因为第一包是带文件名的，不可能是全0包，  数据包是在YMODEM_RX_ACK状态下接收的，不进行全0判断，因此就算文件全为空格，也不会当成结束。

* 通过crt传输文件时，测试板接收到文件，发送C后无响应，以至于ymodem程序无法正常退出，但xshell可以退出。
解决办法： 一次只传一个文件，接收到一个文件后就退出程序。


