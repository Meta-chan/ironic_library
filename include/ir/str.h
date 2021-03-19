/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_STR
#define IR_STR
	
#include "types.h"

namespace ir
{
	template<class T> inline int32 strcmp(const T *s1, const T *s2) noexcept;
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_STR) : !defined(IR_EXCLUDE_STR)
	#ifndef IR_INCLUDE
		#ifndef IR_STR_INLINE_SOURCE
			#define IR_STR_INLINE_SOURCE
			#include "../../source/inline/str.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_STR_INLINE_SOURCE
			#define IR_STR_INLINE_SOURCE
			#include "../../source/inline/str.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_STR_INLINE_SOURCE
			#define IR_STR_INLINE_SOURCE
			#include "../../source/inline/str.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_STR