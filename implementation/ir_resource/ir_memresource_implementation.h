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

void ir::MemResourceFreer::free(void *mem)
{
	if (mem != nullptr) ::free(mem);
};

template<class T> ir::MemResource<T>::MemResource() : Resource<T*, MemResourceFreer, nullptr>::Resource(nullptr)
{
};

template<class T> ir::MemResource<T>::MemResource(T *it) : Resource<T*, MemResourceFreer, nullptr>::Resource(it)
{
};
	
#endif	//#ifndef IR_MEM_RESOURCE_IMPLEMENTATION