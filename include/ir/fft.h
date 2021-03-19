/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FFT
#define IR_FFT

#include <complex>
#include <stddef.h>

namespace ir
{
///@addtogroup math Math functions
///@{
	
	///Fast Fourier Transformation
	///@param data	Pointer to array of complex values that are input and output at the same time
	///@param size	Size of the vector. Needs to be power of two
	///@return true on success, false on fail
	template<class T>bool fft(std::complex<T> *data, size_t size)			noexcept;
	
	///Inverse Fast Fourier Transformation
	template<class T>bool ifft(std::complex<T> *data, size_t size)			noexcept;

	///Inverse Fast Fourier Transformation without dividing on @c size
	template<class T>bool ifft_nodivide(std::complex<T> *data, size_t size)	noexcept;
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_FFT) : !defined(IR_EXCLUDE_FFT)
	#ifndef IR_INCLUDE
		#ifndef IR_FFT_TEMPLATE_SOURCE
			#define IR_FFT_TEMPLATE_SOURCE
			#include "../../source/template/fft.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_FFT_TEMPLATE_SOURCE
			#define IR_FFT_TEMPLATE_SOURCE
			#include "../../source/template/fft.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_FFT