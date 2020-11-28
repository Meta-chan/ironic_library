#define IR_IMPLEMENT
#include <ir_math/ir_fft.h>
#include <ir_math/ir_gauss.h>

int main()
{
	std::complex<float> arr[4] = { {0, 0}, {1, 0}, {2, 0}, {5, 0} };
	ir::fft(arr, 2);
	ir::ifft(arr, 2);

	float **m = new float*[2];
	m[0] = new float[2];
	m[1] = new float[2];
	m[0][0] = 0;
	m[0][1] = 1;
	m[1][0] = 1;
	m[1][1] = 0;

	float *v = new float[2];
	v[0] = 4;
	v[1] = 5;

	ir::gauss(2, m, v);
	return 0;
}