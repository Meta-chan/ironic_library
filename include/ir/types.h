/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_TYPES
#define IR_TYPES

#include <stdint.h>

namespace ir
{
	typedef int8_t int8;
	typedef int16_t int16;
	typedef int32_t int32;
	typedef int64_t int64;
	
	typedef uint8_t uint8;
	typedef uint16_t uint16;
	typedef uint32_t uint32;
	typedef uint64_t uint64;
	
	typedef char char8;
	#ifdef _WIN32
		typedef wchar_t char16;
		typedef wchar_t schar;
		#define SS(_TEXT) L ## _TEXT
	#else
		#if ('\xFF' > 0)
			typedef uint16 char16;				
		#else
			typedef int16 char16;
		#endif
		typedef char schar;
		#define SS(_TEXT) _TEXT
	#endif
	#if ('\xFF' > 0)
		typedef uint32 char32;
	#else
		typedef int32 char32;
	#endif
}

#endif	//#ifndef IR_TYPES