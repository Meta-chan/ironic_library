#define IR_IMPLEMENT
#include <ir_container/ir_register.h>
#include <stdio.h>

template class ir::Register<int, char>;

int main()
{
	ir::Register<int, char> reg;
	reg.set("Sunflower", 1);
	reg.set("The Goddess", 2);
	reg.set("Mercury", 3);
	printf("%i\n", reg["Mercury"]);
};