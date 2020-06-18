/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SYSCHAR
#define IR_SYSCHAR
///@defgroup ir_syschar Syschar
///Ironic library's char
///@{
	
	namespace ir
	{			
		#ifdef _WIN32
			///Ironic library's char
			typedef wchar_t syschar;
			///Identifies a string as string in platform's native encoding
			#define SS(_TEXT) L ## _TEXT
		#else
			///Ironic library's char
			typedef char syschar;
			///Identifies a string as string in platform's native encoding
			#define SS(_TEXT) _TEXT
		#endif
	}
	
///@}
#endif	//#ifndef IR_SYSCHAR