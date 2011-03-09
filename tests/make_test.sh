#!/bin/bash
#first parameter is input file, second is output file
g++ -I/usr/local/gtest/include/ $1.cpp -c
g++ /usr/local/lib/libgtest.so /usr/local/lib/libgtest_main.so $1.o -lpthread -o $2
