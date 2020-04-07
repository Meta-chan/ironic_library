/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SYSCHAR
#define IR_SYSCHAR
	
	namespace ir
	{
		#ifdef _WIN32
			typedef wchar_t syschar;
			#define SS(_TEXT) L ## _TEXT
		#else
			typedef char syschar;
			#define SS(_TEXT) _TEXT
		#endif
	}
	
#endif	//#ifndef IR_SYSCHAR