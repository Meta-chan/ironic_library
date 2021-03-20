#include "../include/ir/fft.h"
#include "../include/ir/types.h"

int main()
{
	std::complex<double> buffer[16];
	buffer[0] = std::complex<double>(1.0, 0.0);
	for (ir::uint32 i = 1; i < 16; i++) buffer[i] = std::complex<double>(0.0, 0.0);
	ir::fft(buffer, 16);
	return 0;
}