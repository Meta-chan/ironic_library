/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <stdio.h>

inline size_t ir::print(const char8 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	#ifdef _WIN32
		size_t printed = vprintf_s(format, args);
	#else
		size_t printed = vprintf(format, args);
	#endif
	va_end(args);
	return printed;
}

inline size_t ir::print(const char8 *format, va_list args) noexcept
{
	#ifdef _WIN32
		return vprintf_s(format, args);
	#else
		return vprintf(format, args);
	#endif
}

inline size_t ir::print(File file, const char8 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	#ifdef _WIN32
		size_t printed = vfprintf_s(file.file(), format, args);
	#else
		size_t printed = vfprintf(file.file(), format, args);
	#endif
	file.seek(printed, SEEK_CUR);
	va_end(args);
	return printed;
}

inline size_t ir::print(File file, const char8 *format, va_list args) noexcept
{
	#ifdef _WIN32
		size_t printed = vfprintf_s(file.file(), format, args);
	#else
		size_t printed = vfprintf(file.file(), format, args);
	#endif
	file.seek(printed, SEEK_CUR);
	return printed;
}

inline size_t ir::print(size_t offset, char8 *buffer, size_t size, const char8 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	#ifdef _WIN32
		size_t printed = vsprintf_s(buffer + offset, size - offset, format, args);
	#else
		size_t printed = vsprintf(buffer + offset, format, args);
	#endif
	va_end(args);
	return printed;
}

inline size_t ir::print(size_t offset, char8 *buffer, size_t size, const char8 *format, va_list args) noexcept
{
	#ifdef _WIN32
		return vsprintf_s(buffer + offset, size - offset, format, args);
	#else
		return vsprintf(buffer + offset, format, args);
	#endif
}

template<size_t A>
inline size_t ir::print(size_t offset, char8 buffer[A], const char8 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	#ifdef _WIN32
		size_t printed = vsprintf_s(buffer + offset, A - offset, format, args);
	#else
		size_t printed = vsprintf(buffer + offset, format, args);
	#endif
	va_end(args);
	return printed;
}

template<size_t A>
inline size_t ir::print(size_t offset, char8 buffer[A], const char8 *format, va_list args) noexcept
{
	#ifdef _WIN32
		return vsprintf_s(buffer + offset, A - offset, format, args);
	#else
		return vsprintf(buffer + offset, format, args);
	#endif
}

#ifdef _WIN32

inline size_t ir::print(const char16 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	size_t printed = vwprintf_s(format, args);
	va_end(args);
	return printed;
}

inline size_t ir::print(const char16 *format, va_list args) noexcept
{
	return vwprintf_s(format, args);
}

inline size_t ir::print(File file, const char16 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	size_t printed = vfwprintf_s(file.file(), format, args);
	file.seek(printed, SEEK_CUR);
	va_end(args);
	return printed;
}

inline size_t ir::print(File file, const char16 *format, va_list args) noexcept
{
	size_t printed = vfwprintf_s(file.file(), format, args);
	file.seek(printed, SEEK_CUR);
	return printed;
}

inline size_t ir::print(size_t offset, char16 *buffer, size_t size, const char16 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	size_t printed = vswprintf_s(buffer + offset, size - offset, format, args);
	va_end(args);
	return printed;
}

inline size_t ir::print(size_t offset, char16 *buffer, size_t size, const char16 *format, va_list args) noexcept
{
	return vswprintf_s(buffer + offset, size - offset, format, args);
}

template<size_t A>
inline size_t ir::print(size_t offset, char16 buffer[A], const char16 *format, ...) noexcept
{
	va_list args;
	va_start(args, format);
	size_t printed = vswprintf_s(buffer + offset, A - offset, format, args);
	va_end(args);
	return printed;
}

template<size_t A>
inline size_t ir::print(size_t offset, char16 buffer[A], const char16 *format, va_list args) noexcept
{
	return vswprintf_s(buffer + offset, A - offset, format, args);
}

#endif