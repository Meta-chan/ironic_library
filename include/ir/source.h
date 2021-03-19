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

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_SOURCE) : !defined(IR_EXCLUDE_SOURCE)
	#ifndef IR_INCLUDE
		#ifndef IR_SOURCE_INLINE_SOURCE
			#define IR_SOURCE_INLINE_SOURCE
			#include "../../source/inline/source.h"
		#endif
		#ifndef IR_SOURCE_TEMPLATE_SOURCE
			#define IR_SOURCE_TEMPLATE_SOURCE
			#include "../../source/template/source.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_SOURCE_INLINE_SOURCE
			#define IR_SOURCE_INLINE_SOURCE
			#include "../../source/inline/source.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_SOURCE_INLINE_SOURCE
			#define IR_SOURCE_INLINE_SOURCE
			#include "../../source/inline/source.h"
		#endif
		#ifndef IR_SOURCE_TEMPLATE_SOURCE
			#define IR_SOURCE_TEMPLATE_SOURCE
			#include "../../source/template/source.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_SOURCE