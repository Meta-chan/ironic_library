/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_HINTERNET_RESOURCE
#define IR_HINTERNET_RESOURCE

#include <ir_resource/ir_resource.h>
#include <Winhttp.h>

namespace ir
{
///@addtogroup ir_resource
///@{
	
	#ifndef DOXYGEN
		class HInternetIniterFreer
		{
		private:
			static HINTERNET initvalue;
			static void free(HINTERNET hinternet);
			friend Resource<HINTERNET, HInternetIniterFreer>;
		};
	#endif
	
	///Windows native Internet handle resource
	///Acts almost like Windows native Internet handle
	///If it's value is not NULL or INVALID_HANDLE_VALUE at the end of the visibility range, the handle is closed
	typedef Resource<HINTERNET, HInternetIniterFreer> HInternetResource;
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_HINTERNET_RESOURCE_IMPLEMENT)) && !defined(IR_HINTERNET_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_hinternet_resource_implementation.h>
#endif

#endif	//#ifndef IR_HINTERNET_RESOURCE