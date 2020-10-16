#define IR_IMPLEMENT
#include <ir_container/ir_string.h>
#include <stdio.h>

int _main()
{
	ir::String s = "Hello";
	s += " ";
	s += "World";
	printf("%s\n", s.data());
	printf("%s\n", s == "Hello World" ? "Equal" : "Not equal");
	return 0;
};

int main()
{
	return _main();
};