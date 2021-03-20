/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <string.h>
#include <wchar.h>

template<class T>
inline ir::int32 ir::strcmp(const T *s1, const T *s2) noexcept
{
	if (sizeof(T) == 1) return ::strcmp((const char*)s1, (const char*)s2);
	else if (sizeof(T) == 2) return ::wcscmp((const wchar_t*)s1, (const wchar_t*)s2);
	else for (size_t i = 0; true; i++) { if (s1[i] != s2[i]) return ((int)s1[i] - (int)s2[i]); }
	return 0;
}