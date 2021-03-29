/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FNV1A
#define IR_FNV1A

#include "block.h"
#include "types.h"

namespace ir
{
///@addtogroup crypt Cryptography
///@{
	
	uint32 fnv1a(Block key) noexcept;
	
///@}
}

#endif	//#ifndef IR_FNV1A

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_FNV1A) : !defined(IR_EXCLUDE_FNV1A)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_FNV1A_SOURCE
			#define IR_FNV1A_SOURCE
			#include "../../source/fnv1a.h"
		#endif
	#endif
#endif