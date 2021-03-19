#!/bin/sh
test()
{
	
	if g++ $1 -I .. -Wall -Wextra -march=native -std=c++11 -o $1.exe
	then
		GREEN='\033[0;32m'
		WHITE='\033[0m'
		printf "I ${GREEN}Success${WHITE}\n"
		return 0
	else
		RED='\033[0;31m'
		WHITE='\033[0m'
		printf "I ${RED}Fail${WHITE}\n"
		return 1
	fi
}

for f in ./*
do
	if [ $f!=rgb.c && $f!=ir_plot_test.cpp ]
	then
		if [ !test $f ]
		then
			break
		fi
	fi
done

rm *.exe -f
rm *.obj -f