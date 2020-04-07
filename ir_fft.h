/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FFT
#define IR_FFT

#include <complex.h>

namespace ir
{
	void fft(_Dcomplex *data, unsigned int N);
	void ifft(_Dcomplex *data, unsigned int N);

	#if defined(IR_IMPLEMENT) || defined(IR_FFT_IMPLEMENT)
		#include "implementation/ir_fft_implementation.h"
	#endif
};

#endif	//#ifndef IR_FFT