#define IR_IMPLEMENT
#include <ir_container/ir_quiet_register.h>
#include <stdio.h>

template class ir::QuietRegister<int, char>;

int main()
{
	ir::QuietRegister<int, char> reg;
	reg("Sunflower") = 1;
	reg("The Goddess") = 2;
	reg("Mercury") = 3;
	printf("%i\n", reg["Mercury"]);
};