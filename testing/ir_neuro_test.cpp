#define IR_IMPLEMENT
#include "ir_neuro.h"
#include <random>
#include <stdio.h>

int main()
{
	//simple net trying to learn xor operation

	float input[2];
	float output, goal;

	ir::ec code;
	unsigned int lays[3] = { 2, 2, 1 };
	ir::Neuro net(3, lays, &code);
	
	for (unsigned int i = 0; i < 100000; i++)
	{
		char b[2];
		b[0] = rand() % 2;
		b[1] = rand() % 2;
		input[0] = 2 * ((float) b[0] - 0.5f);
		input[1] = 2 * ((float) b[1] - 0.5f);
		goal = 2 * ((float) (b[0] ^ b[1]) - 0.5f);

		net.forward(input, &output, false, false);
		net.backward(input, &output, &goal, 1.0f);
		printf("%i %i -> %f\n", b[0], b[1], output);
	}

	getchar();
};