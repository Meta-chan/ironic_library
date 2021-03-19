#include "../include/ir/gauss.h"

int main()
{
	double A1[2] = { 1, 2};
	double A2[2] = { 3, 4 };
	double *A[2] = { A1, A2 };
	double b[2] = { 5, 6 };
	double x[2];
	return ir::gauss(2, A, b, x) ? 0 : 1;
}