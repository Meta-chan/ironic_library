/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_EC
#define IR_EC

#include "types.h"

namespace ir
{
///@addtogroup definitions Definitions
///@{
	
	///Ironic library's standard return code
	enum class ec : uint32
	{
		ok = 0,					///< Indicates success
		other = 1,				///< Undocumented, rare or platform-specific error
		not_implemented = 2,	///< Function is not yet implemented

		//common sector
		alloc,					///< Memory allocation failed
		null,					///< One of arguments is `nullptr` while it is not allowed
		create_file,			///< Opening file with write access failed
		open_file,				///< Opening file with read access failed
		read_file,				///< Reading from file failed
		write_file,				///< Writing to file failed
		seek_file,				///< Seeking in file failed
		invalid_signature,		///< Signature or size of file is invalid
		object_not_inited,		///< Initialization of object has failed, the object can not be used
		align,					///< Memory is not properly aligned
		invalid_input,			///< Input or arguments are obviously invalid

		//Windows specific sector
		windows_register_class,	///< RegisterClass function failed, (Windows-specific)
		windows_create_window,	///< CreateWindow function failed (Windows-specific)
		windows_getmessage,		///< GetMessage function failed (Windows-specific)
		windows_createthread,	///< CreateThread function failed (Windows-specific)

		//Mapping sector
		mapping,				///< `ir::Mapping::map` failed

		//databases and registers sector
		key_not_exists,			///< Identifier is not found in container (logical error)
		key_already_exists,		///< Identifier is found in container (logical error)
	};
	
///@}
}

#endif //ifndef IR_EC

/**@mainpage Welcome to Ironic Library!

@tableofcontents

@section Overview
Here is a brief but full overview of features of the library:
 - Containers: `block.h`, `map.h`, `quiet_hash_map.h`, `quiet_list.h`, `quiet_map.h`, `quiet_ring.h`, `quiet_vector.h`, `string.h`, `vector.h`
 - Definitions: `constants.h`, `types.h`
 - Encoding library: `encoding.h`
 - Mathematics: `fft.h`, `gauss.h`
 - File utilities: `file.h`, `mapping.h`
 - Hash algorithms: `fnv1a.h`, `md5.h`
 - Networking: `ip.h`, `tcp.h`, `udp.h`
 - Databases: `n2st_database.h`, `s2st_database.h`
 - Neuronal networks: `neuro.h`
 - High-performance computing: `parallel.h`, `matrix.h`
 - RAII wrapper: `resource.h`
 - Source and sink abstractions: `sink.h`, `source.h`
 - Cross-platform versions of standard C functions: `str.h`, `print.h`
 
@section Installation
Installation was designed to be super-easy and super-flexible at the same time.

Easy way: templates and inlines will work right out of the box. To use non-template and non-inline methods, include `ironic.cpp` to your project.

Flexible way: there is a way to reduce compile time and compile parts Ironic library with different options, but for this a little theory is needed. The code is divided into three groups:
 - Inline \- it's implementation shall always be present in each `.cpp` file.
 - Template \- it's implementation shall always be present **and used** in one or more `.cpp` files. Note that `template class ir::EXAMPLE<int>` is also an "usage" and will force the compiler to compile all methods of `EXAMPLE<int>` class.
 - Non-inline and non-template \- it's implementation shell always be present in one `.cpp` file.

So the modes of compilation and correspondent macros are:
 - <tt> #define IR_INCLUDE 'n' </tt> will include no implementations. The compiler might complain about `inline function is not implemented` or equivalent warning.
 - <tt> #define IR_INCLUDE 'i' </tt> will include only implementation of inline functions.
 - <tt> #define IR_INCLUDE 't' </tt> will include implementation of inline and template functions (default behavior).
 - <tt> #define IR_INCLUDE 'a' </tt> will include include all implementations.

Also if you define `IR_EXCLUDE_%EXAMPLE%` the `%EXAMPLE%` implementation will be not included. But if you define `IR_EXCLUDE`, the compiler will include **only** implementations marked with `IR_INLCUDE_%EXAMPLE%`.

@section Platforms
The code is tested for Windows x86 and x64, Linux x64 and Linux ARMv7.

@section Natvis
For some classes [Natvis](https://docs.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects) files are provided. Include these files to your Visual Studio project and enjoy debugging.

@section Documentation
[Doxygen](https://www.doxygen.nl/manual/starting.html) documentation is provided. I would recommend to start with **Modules** page.

###### P.S. My code is not dirty, it is alternatively clean.
*/