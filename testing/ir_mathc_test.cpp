/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#define IR_IMPLEMENT
//#define IR_MATHC_THREADS 4

#include <ir_math/ir_mathc.h>
#include <random>
#include <time.h>

#define SIZE 1000
#define N 100
#define ALIGN 4

void test()
{
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	bool ok;
	
	ir::MatrixC<float, ALIGN> ac(SIZE, SIZE, &ok); if (!ok) return;
	for (unsigned int i = 0; i < SIZE; i++)
		for (unsigned int j = 0; j < SIZE; j++)	ac.data(i)[j] = distribution(generator);

	ir::VectorC<float, ALIGN> bc(SIZE, &ok); if (!ok) return;
	for (unsigned int i = 0; i < SIZE; i++)	bc.data()[i] = distribution(generator);

	ir::VectorC<float, ALIGN> cc(SIZE, &ok); if (!ok) return;
	
	ir::MathC<float, ALIGN>::multiply_mvv(&ac, &bc, &cc);
	clock_t cl = clock();
	for (unsigned int i = 0; i < N; i++)
	{
		ir::MathC<float, ALIGN>::multiply_mvv(&ac, &bc, &cc);
	}
	printf("Done in %f seconds\n", (float)(clock() - cl)/CLOCKS_PER_SEC);

	return;
};

int main(int argc, char **argv)
{
	test();
	getchar();
	return 0;
};
