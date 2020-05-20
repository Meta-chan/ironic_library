/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license filedes to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FILEDES_RESOURCE
#define IR_FILEDES_RESOURCE

#include <ir_resource/ir_resource.h>
#include <unistd.h>

namespace ir
{
	class FiledesIniterFreer
	{
	public:
		static int initvalue;
		static void free(int filedes);
	};
	
	typedef Resource<int, FiledesIniterFreer> FileResource;
};

#if (defined(IR_IMPLEMENT) || defined(IR_FILEDES_RESOURCE_IMPLEMENT)) && !defined(IR_FILEDES_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_filedes_resource_implementation.h>
#endif

#endif	//#ifndef IR_FILEDES_RESOURCE