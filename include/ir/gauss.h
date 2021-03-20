/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_GAUSS
#define IR_GAUSS
	
#include <stddef.h>

namespace ir
{
///@addtogroup math Mathematics
///@{

	///Gauss algorithm of solving linear equations system
	///@param size		Dimension of matrix and vector
	///@param matrix	Square matrix of coefficients of linear system. May be changed
	///@param vector	Input of the algorithm. May be changed. Receives result if `result == nullptr`
	///@param result	Vector that receives result. Receives result if not `nullptr`
	///@return			true if the matrix was regular, false if singulars
	template<class T> bool gauss(size_t size, T **matrix, T *vector, T *result = (T*)nullptr) noexcept;
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_GAUSS) : !defined(IR_EXCLUDE_GAUSS)
	#ifndef IR_INCLUDE
		#ifndef IR_GAUSS_TEMPLATE_SOURCE
			#define IR_GAUSS_TEMPLATE_SOURCE
			#include "../../source/template/gauss.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_GAUSS_TEMPLATE_SOURCE
			#define IR_GAUSS_TEMPLATE_SOURCE
			#include "../../source/template/gauss.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_GAUSS