/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Gauss algorithm. matrix and vector will be changed.
//If result is nullptr, result is written to vector.

#ifndef IR_GAUSS
#define IR_GAUSS

namespace ir
{
	template<class T> bool gauss(size_t size, T **matrix, T *vector, T *result = (T*)nullptr);
};

#if (defined(IR_IMPLEMENT) || defined(IR_GAUSS_IMPLEMENT)) && !defined(IT_GAUSS_NOT_IMPLEMENT)
	#include <implementation/ir_math/ir_gauss_implementation.h>
#endif

#endif	//#ifndef IR_GAUSS