#define IR_IMPLEMENT
#include <ir_container/ir_list.h>
#include <stdio.h>
#include <list>

template class ir::List <unsigned int>;

int print(const ir::List<unsigned int> list)
{
	for (unsigned int i = 0; i < list.size(); i++) printf("%u ", list[i]);
	return 0;
}

int nullate(ir::List<unsigned int> list)
{
	for (unsigned int i = 0; i < list.size(); i++) list[i] = 0;
	return 0;
}

int _main()
{
	ir::List<unsigned int> list(10);
	for (unsigned int i = 0; i < 10; i++)
	{
		list[i] = i;
	}
	print(list);
	nullate(list);
	return 0;
}

int main()
{
	return _main();
}