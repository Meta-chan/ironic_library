#define IR_IMPLEMENT
#include <ir_neuro.h>
#include <random>
#include <stdio.h>

int main()
{
	//simple net trying to learn xor operation

	ir::ec code;
	unsigned int lays[3] = { 2, 2, 1 };
	ir::Neuro<ir::TanhFunction, 2> net(3, lays, 0.5f, &code);
	
	net.set_coefficient(1.0f);
	net.set_inductance(100.0f);
	
	for (unsigned int i = 0; i < 100000; i++)
	{
		char b[2];
		b[0] = rand() % 2;
		b[1] = rand() % 2;
		float input[2] = { 2 * ((float)b[0] - 0.5f), 2 * ((float)b[1] - 0.5f) };
		float goal = 2 * ((float)(b[0] ^ b[1]) - 0.5f);
		float output;

		net.set_input(input, true);
		net.set_output_pointer(&output, true);
		net.forward();
		net.get_output();
		net.set_goal(&goal, true);
		net.backward();

		printf("%i %i -> %f\n", b[0], b[1], output);
	}

	net.save(SS("xor.inr"));
	getchar();
};