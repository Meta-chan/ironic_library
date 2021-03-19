/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MD5
#define IR_MD5

#include <stddef.h>

namespace ir
{
///@addtogroup crypto Cryptographics
///@{

	///MD5 hash algorithm
	///@param[in]	data		Data to hash
	///@param[in]	bitlength	Length of data in bits
	///@param[out]	hash		Pointer to 16-bytes memory block to recieve the hash.
	void md5(const void *data, size_t bitlength, void *hash) noexcept;

///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_MD5) : !defined(IR_EXCLUDE_MD5)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_MD5_INLINE_SOURCE
			#define IR_MD5_SOURCE
			#include "../../source/md5.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_MD5