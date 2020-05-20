/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MEM_RESOURCE_IMPLEMENTATION
#define IR_MEM_RESOURCE_IMPLEMENTATION

#include <stdlib.h>

template <class T> T *ir::MemIniterFreer<T>::initvalue = nullptr;

template<class T> void ir::MemIniterFreer<T>::free(T *mem)
{
	if (mem != nullptr) ::free(mem);
};

#endif	//#ifndef IR_MEM_RESOURCE_IMPLEMENTATION