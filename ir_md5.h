/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MD5
#define IR_MD5

#ifdef __cplusplus
	namespace ir
	{
		void md5(const void *data, unsigned int bitlength, void *hash);
	}
#else
	void ir_md5(const void *data, unsigned int bitlength, void *hash);
#endif

//data - input
//bitlength - length in bits
//hash - output, must point to some 16 bytes of memory.
//It could be unsigned int hash[4]
//Or unsigned char hash[16]

#if (defined(IR_MD5_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include "ir_md5_implementation.h"
#endif

#endif //#ifndef IR_MD5