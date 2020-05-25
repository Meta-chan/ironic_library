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

#define _USE_MATH_DEFINES
#include <complex>
#include <stddef.h>

namespace ir
{
	template<class T>bool fft(std::complex<T> *data, size_t size);
	template<class T>bool ifft(std::complex<T> *data, size_t size);

	#if (defined(IR_IMPLEMENT) || defined(IR_FFT_IMPLEMENT)) && !defined(IR_FFT_NOT_IMPLEMENT)
		#include <implementation/ir_math/ir_fft_implementation.h>
	#endif
};

#endif	//#ifndef IR_FFT