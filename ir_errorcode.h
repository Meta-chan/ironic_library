/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_ERRORCODE
#define IR_ERRORCODE
	
namespace ir
{
///@defgroup ir_errorcode Error codes
///Ironic library's standard return code, indicates whether the function call succeed @n
///Some codes are already assigned value to and will never change. Other are not fest and may be edited with time. You may need to recompile modules of you program that use ir_errorcode.h to ensure you get correct error codes
///@{

	///Ironic library's standard return code
	enum ec : unsigned int
	{
		ec_ok = 0,					///< Indicates success
		ec_other = 1,				///< Undocumented, rare or platform-specific error
		ec_not_implemented = 2,		///< Function is not yet implemented

		//common sector
		ec_alloc,					///< Memory allocation failed
		ec_null,					///< One of arguments is nullptr while it is not allowed
		ec_create_file,				///< Opening file with write access failed
		ec_open_file,				///< Opening file with read access failed
		ec_read_file,				///< Reading from file failed
		ec_write_file,				///< Writing to file failed
		ec_seek_file,				///< Seeking in file failed
		ec_invalid_signature,		///< Signature or size of file is invalid
		ec_object_not_inited,		///< Initialization of object has failed, the object can not be used
		ec_align,					///< Memory is not properly aligned
		ec_invalid_input,			///< Input or arguments are obviously invalid

		//Windows specific sector
		ec_windows_register_class,	///< RegisterClass function failed, (Windows-specific)
		ec_windows_create_window,	///< CreateWindow function failed (Windows-specific)
		ec_windows_getmessage,		///< GetMessage function failed (Windows-specific)
		ec_windows_createthread,	///< CreateThread function failed (Windows-specific)

		//openmap sector
		ec_openmap,					///< @c ir::openmap function failed

		//databases and registers sector
		ec_key_not_exists,			///< Identifier is not found in container (logical error)
		ec_key_already_exists,		///< Identifier is found in container (logical error)

		//opengl sector
		ec_opengl					///< OpenGL function failed
	};
	
	
///@}
};

#endif //ifndef IR_ERRORCODE

/** @mainpage Welcome to Ironic Library!
*The code presented here is some pieces of my home projects. Please visit @b Modules to begin getting with the project!
*
*### Platforms
*The code is developed and tested mostly under Windows x86. Most of libraries were specially tested under Linux x86. Some are cross-platform by chance. Some need just several corrections and renamings to make them work on your platform. Some are really stuck to WinAPI, some may crash on ARM. It that case, I beg your pardon.
* 
*### How to install?
*I wanted to make the installation really simple. The simplest way to install the library is to choose one of .cpp files and paste 
*```
#define IR_IMPLEMENT
#include <ir_NAME.h>
*```
*
*If you want to manage yourself, which files to compile (for example, you want to compile one part as debug, one as release), you can also use these symbols:
*```
#define IR_NAME_IMPLEMENT
#define IR_NAME_NOT_IMPLEMENT
*```
*
*Templates are quite non-ironic-way thing, and there are difficulties. The Ironic way of using templates is (on example of vector):
*```
#define IR_IMPLEMENT
#include <ir_vector.h>
template class ir::vector<float>;
*```
*
*And after doing so you can use your `ir::vector<float>` in every file. But actually you can just include the header in every file, the compiler is smart enough and does not complain about multiple implementations (just like in STL). But doing so will increase the compile time.
*
*### About Natvis
*For some classes I provide Natvis files! Include theese files to your Visual Studio project and enjoy debugging. [More](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects).
*
*###### P.S. My code is not dirty, it is alternatively clean.
*/