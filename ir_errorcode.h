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
	};
	
	
///@}
};

#endif //ifndef IR_ERRORCODE

/**@mainpage Welcome to Ironic Library!
*The code presented here is some pieces of my home projects. Here you will find:
* - ir_database		- ultra-light databases
* - ir_neuro		- neural network
* - ir_resource		- helpful class wrappers around non-class things (RAII)
* - ir_utf			- encoding library
* - ir_container	- library's containers, vectors, blocks, registers
* - ir_math			- some math algorithms
* - ir_md5			- MD5 hash algorithm
* - ir_openmap		- mapping function
* - ir_plot			- plotting function
*
*### Platforms
*The code is developed and tested mostly under Windows x86. Most of libraries were specially tested under Linux x86. Some are cross-platform by chance. Some need just several corrections and renamings to make them work on your platform. Some are really stuck to WinAPI, some may crash on ARM, POWER, SPARC etc. In that case, I beg your pardon.
* 
*### How to install?
*I wanted to make the installation really simple. The simplest way to install the library is to choose one of `.cpp` files, defile `IR_IMPLEMENT` and include all libraries you are interested in. After doing so you can use headers in every file like header-only libraries.
*@code{.cpp}
#define IR_IMPLEMENT
#include <ir_EXAMPLE.h>
*@endcode
*
*Include principles can be more flexible, but you need to understand how they works. `IR_IMPLEMENT` makes the compiler to include all implementations, `IR_EXAMPLE_IMPLEMENT` makes it include specific implementation, `IR_EXAMPLE_NOT_IMPLEMENT` makes it exclude one. With this mechanism you can control compiler options which you compile Ironic code with and solve linker errors. For clarity, typical Ironic header looks like this:
*@code{.cpp}
//ir_example.h
int example();
#if (defined(IR_IMPLEMENT) || defined(IR_EXAMPLE_IMPLEMENT)) && !defined(IR_EXAMPLE_NOT_IMPLEMENT)
	#include <implementation/ir_example_implementation.h>
#endif
*@endcode
*
*Templates are quite non-ironic-way thing, and there are difficulties. The Ironic way of using templates is (on example of `ir::Vector`):
*@code{.cpp}
#define IR_IMPLEMENT
#include <ir_vector.h>
template class ir::Vector<float>;
*@endcode
*
*After doing so you can use your `ir::Vector<float>` in every file. But actually you can define `IR_VECTOR_IMPELMENT` and include `ir_vector.h` in every file, and use `ir::Vector` with any template parameters (almost like you do with STL). The compiler is smart enough and does not complain about multiple implementations. But doing so will increase the compile time.
*
*### How to get help?
*The code is pretty self-documented. But more importantly, I provide [Doxygen](https://www.doxygen.nl/manual/starting.html) documentation! It does not look too pretty since I am not an expert, but it is still quite informative. I would recommend to start with **Modules** page. And of course, feel free to contact me!
*
*### About Natvis
*For some classes I provide [Natvis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) files! Include these files to your Visual Studio project and enjoy debugging.
*
*###### P.S. My code is not dirty, it is alternatively clean.
*/