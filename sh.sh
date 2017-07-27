#!/bin/bash
make clean
make
scp receive root@10.10.1.49:~/ymodem/
