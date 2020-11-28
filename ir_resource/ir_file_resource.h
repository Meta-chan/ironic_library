/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FILE_RESOURCE
#define IR_FILE_RESOURCE

#include <ir_resource/ir_resource.h>
#include <stdio.h>

namespace ir
{
///@addtogroup ir_resource
///@{
		
	#ifndef DOXYGEN
		class FileIniterFreer
		{
		private:
			static FILE *initvalue;
			static void free(FILE *file);
			friend Resource<FILE*, FileIniterFreer>;
		};
	#endif
	
	///File resource
	///Acts almost like standard C file FILE*
	///If it's value is not nullptr at the end of the visibility range, file is closed
	typedef Resource<FILE*, FileIniterFreer> FileResource;
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_FILE_RESOURCE_IMPLEMENT)) && !defined(IR_FILE_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_file_resource_implementation.h>
#endif

#endif	//#ifndef IR_FILE_RESOURCE