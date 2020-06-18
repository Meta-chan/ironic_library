/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_GAUSS
#define IR_GAUSS
	
#include <stddef.h>

namespace ir
{
///@addtogroup ir_math
///@{

	///Gauss algorithm of solving linear equations system
	///@param size		Dimension of matrix and vector
	///@param matrix	Square matrix of coefficients of linear system. May be changed
	///@param vector	Input of the algorithm. May be changed. Receives result if result == nullptr
	///@param result	Vector that receives result. Receives result if not nullptr
	///@return			true if the matrix was regular, false if singulars
	template<class T> bool gauss(size_t size, T **matrix, T *vector, T *result = (T*)nullptr);
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_GAUSS_IMPLEMENT)) && !defined(IT_GAUSS_NOT_IMPLEMENT)
	#include <implementation/ir_math/ir_gauss_implementation.h>
#endif

#endif	//#ifndef IR_GAUSS