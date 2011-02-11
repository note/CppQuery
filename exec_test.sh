#!/bin/bash
#first parameter is input file, second is output file
g++ -I/usr/local/gtest/include/ $1.cpp -c
if [ $? -eq 0 ]; then
	g++ /usr/local/lib/libgtest.so /usr/local/lib/libgtest_main.so $1.o -lpthread -o $2
	if [ $? -eq 0 ]; then
		./$2
	else
		echo "Executing aborted. Building executable does not succeeded"
	fi
else
	echo "Building aborted. Compiling $1.cpp does not succeeded."
fi

