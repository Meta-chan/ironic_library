/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_PRINT
#define IR_PRINT

#include "types.h"
#include "file.h"
#include <stdarg.h>
#include <stddef.h>

namespace ir
{
///@addtogroup utils Utilities
///@{
	
	///Prints to `stdout`
	inline size_t print(const char8 *format, ...)																noexcept;
	///Prints to `stdout` with `va_list`
	inline size_t print(const char8 *format, va_list args)														noexcept;
	///Prints to file
	inline size_t print(File file, const char8 *format, ...)													noexcept;
	///Prints to file with `va_list`
	inline size_t print(File file, const char8 *format, va_list args)											noexcept;
	///Prints to memory
	inline size_t print(size_t offset, char8 *buffer, size_t size, const char8 *format, ...)					noexcept;
	///Prints to memory with `va_list`
	inline size_t print(size_t offset, char8 *buffer, size_t size, const char8 *format, va_list args)			noexcept;
	///Prints to memory with buffer size autodetection
	template<size_t A> inline size_t print(size_t offset, char8 buffer[A], const char8 *format, ...)			noexcept;
	///Prints to memory with buffer size autodetection with `va_list`
	template<size_t A> inline size_t print(size_t offset, char8 buffer[A], const char8 *format, va_list args)	noexcept;
	
	#ifdef _WIN32
		///Prints to `stdout`
		inline size_t print(const char16 *format, ...)																noexcept;
		///Prints to `stdout` with `va_list`
		inline size_t print(const char16 *format, va_list args)														noexcept;
		///Prints to file
		inline size_t print(File file, const char16 *format, ...)													noexcept;
		///Prints to file with `va_list`
		inline size_t print(File file, const char16 *format, va_list args)											noexcept;
		///Prints to memory
		inline size_t print(size_t offset, char16 *buffer, size_t size, const char16 *format, ...)					noexcept;
		///Prints to memory with `va_list`
		inline size_t print(size_t offset, char16 *buffer, size_t size, const char16 *format, va_list args)			noexcept;
		///Prints to memory with buffer size autodetection
		template<size_t A> inline size_t print(size_t offset, char16 buffer[A], const char16 *format, ...)			noexcept;
		///Prints to memory with buffer size autodetection with `va_list`
		template<size_t A> inline size_t print(size_t offset, char16 buffer[A], const char16 *format, va_list args)	noexcept;
	#endif
}

#endif	//#ifndef IR_PRINT

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_PRINT) : !defined(IR_EXCLUDE_PRINT)
	#ifndef IR_INCLUDE
		#ifndef IR_PRINT_INLINE_SOURCE
			#define IR_PRINT_INLINE_SOURCE
			#include "../../source/inline/print.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_PRINT_INLINE_SOURCE
			#define IR_PRINT_INLINE_SOURCE
			#include "../../source/inline/print.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_PRINT_INLINE_SOURCE
			#define IR_PRINT_INLINE_SOURCE
			#include "../../source/inline/print.h"
		#endif
	#endif
#endif