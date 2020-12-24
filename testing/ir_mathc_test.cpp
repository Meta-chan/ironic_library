/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#define IR_IMPLEMENT
//#define IR_MATHC_OPENMP

#include <ir_math/ir_mathc.h>
#include <time.h>

#define TYPE double
#define SIZE 10000
#define N 10
#define ALIGN 16

void test()
{
	ir::MatrixC<TYPE, ALIGN> ac(SIZE, SIZE);
	if (!ac.ok()) return;
	ac.assign_random(-1.0, 1.0);

	ir::VectorC<TYPE, ALIGN> bc(SIZE);
	if (!bc.ok()) return;
	bc.assign_random(-1.0, 1.0);

	ir::VectorC<TYPE, ALIGN> cc(SIZE);
	if (!cc.ok()) return;
	cc.assign_random(-1.0, 1.0);

	cc.assign_mul(&ac, &bc);
	clock_t cl = clock();
	for (unsigned int i = 0; i < N; i++)
	{
		cc.assign_mul(&ac, &bc);
	}
	printf("Done in %f seconds\n", (float)(clock() - cl)/CLOCKS_PER_SEC);

	return;
}

int main()
{
	test();
	getchar();
	return 0;
}