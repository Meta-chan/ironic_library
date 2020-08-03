#define IR_IMPLEMENT
#include <ir_container/ir_vector.h>
#include <ir_container/ir_quiet_vector.h>
#include <stdio.h>
#include <vector>

template class ir::Vector <unsigned int>;
template class ir::QuietVector <unsigned int>;

int print(const ir::Vector<unsigned int> vector)
{
	for (unsigned int i = 0; i < vector.size(); i++) printf("%u ", vector[i]);
	return 0;
};

int nullate(ir::Vector<unsigned int> vector)
{
	for (unsigned int i = 0; i < vector.size(); i++) vector[i] = 0;
	return 0;
};

int _main()
{
	std::vector<unsigned int> v(10);
	ir::Vector<unsigned int> vector(10);
	for (unsigned int i = 0; i < 10; i++)
	{
		vector[i] = i;
	}
	print(vector);
	nullate(vector);
	return 0;
};

int main()
{
	return _main();
};