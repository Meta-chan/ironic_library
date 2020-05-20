/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MEM_RESOURCE
#define IR_MEM_RESOURCE

#include <ir_resource/ir_resource.h>

namespace ir
{
	template <class T> class MemIniterFreer
	{
	public:
		static T *initvalue;
		static void free(T *mem);
	};
	
	template <class T> using MemResource =  Resource<T*, MemIniterFreer<T>>;
};

#if (defined(IR_IMPLEMENT) || defined(IR_MEM_RESOURCE_IMPLEMENT)) && !defined(IR_MEM_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_memresource_implementation.h>
#endif

#endif	//#ifndef IR_MEM_RESOURCE