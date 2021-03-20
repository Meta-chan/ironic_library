#!/bin/sh
compile()
{
	
	if g++ $1 -Wall -Wextra -pedantic -std=c++11 -lgomp -o $1.exe
	then
		GREEN='\033[0;32m'
		WHITE='\033[0m'
		printf "${GREEN}Success${WHITE}\n"
		return 0
	else
		RED='\033[0;31m'
		WHITE='\033[0m'
		printf "${RED}Fail${WHITE}\n"
		return 1
	fi
}

for f in ./*.cpp
do
	compile $f
	if [ $? != 0 ]
	then
		break
	fi
done

rm *.exe -f
rm *.obj -f
