/*
	Part of the MathG Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include "../include/ir/matrix.h"
#include <time.h>

#define TYPE float
#define SIZE 1000
#define ALIGN 16

int _main()
{
	ir::Matrix<TYPE, ALIGN> a(SIZE, SIZE);
	if (!a.ok()) return 1;
	a.random(-1.0, 1.0);

	ir::Matrix<TYPE, ALIGN> b(SIZE, SIZE);
	if (!b.ok()) return 1;
	b.random(-1.0, 1.0);

	ir::Matrix<TYPE, ALIGN> c(SIZE, SIZE);
	if (!c.ok()) return 1;

	//Scalar code:
	clock_t cloc = clock();
	for (size_t row = 0; row < SIZE; row++)
	{
		for (size_t column = 0; column < SIZE; column++)
		{
			TYPE sum = 0.0;
			for (size_t i = 0; i < SIZE; i++)
			{
				sum += a(row, i) * b(column, i);
			}
			c(row, column) = sum;
		}
	}
	printf("Scalar code done in %f seconds\n", (float)(clock() - cloc) / CLOCKS_PER_SEC);

	//(Theoretically) vectorized code:
	cloc = clock();
	c.matrix_product_transposed(&a, &b);
	printf("Vectrized code done in %f seconds\n", (float)(clock() - cloc) / CLOCKS_PER_SEC);

	getchar();
	return 0;
}

int main()
{
	return _main();
}