#define IR_IMPLEMENT
#include "../include/ir/string.h"
#include <stdio.h>

int _main()
{
	ir::String<char> s = "Hello";
	s += " ";
	s += "This wonderfull world";
	printf("%s\n", s.data());
	printf("%s\n", s == "Hello World" ? "Equal" : "Not equal");
	return 0;
}

int main()
{
	return _main();
}