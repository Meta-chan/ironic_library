/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SOURCE
#define IR_SOURCE

#include "types.h"
#include <stddef.h>

namespace ir
{
///@addtogroup container Containers
///@{
	
	///Class to read data from
	class Source
	{
		virtual bool limited()						const noexcept = 0;
		virtual size_t available()					const noexcept = 0;
		virtual size_t read(void *data, size_t size)noexcept = 0;
		virtual ~Source()							noexcept = 0;
	};
	
///@}
}

#endif	//#ifndef IR_SOURCE

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_SOURCE) : !defined(IR_EXCLUDE_SOURCE)
	#ifndef IR_INCLUDE
	#elif IR_INCLUDE == 'a'
		#ifndef IR_SOURCE_SOURCE
			#define IR_SOURCE_SOURCE
			#include "../../source/source.h"
		#endif
	#endif
#endif