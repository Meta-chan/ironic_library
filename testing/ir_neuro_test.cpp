#define IR_IMPLEMENT
#include <ir_neuro.h>
#include <random>
#include <stdio.h>

template class ir::Neuro<float, 1, ir::Tanh<float>>;

int main()
{
	//simple net trying to learn xor operation

	unsigned int lays[3] = { 2, 2, 1 };
	ir::Neuro<> net(3, lays, 0.5, nullptr);
	
	net.set_coefficient(0.05);
	
	for (unsigned int i = 0; i < 10000; i++)
	{
		bool a = rand() % 2;
		bool b = rand() % 2;
		bool r = a ^ b;

		net.get_input()->data()[0] = a ? 1 : -1;
		net.get_input()->data()[1] = b ? 1 : -1;
		net.forward();
		printf("%i %i -> %f\n", a ? 1 : -1, b ? 1 : -1, net.get_output()->data()[0]);

		net.get_goal()->data()[0] = r ? 1 : -1;
		net.backward();
	}

	net.save(SS("xor.inr"));
	getchar();
};