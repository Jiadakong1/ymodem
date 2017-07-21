#!/bin/bash
make clean
make
scp -r ../ymodem root@10.10.1.49:~
