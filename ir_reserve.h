/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RESERVE
#define IR_RESERVE
	
unsigned char reserve(void **pdata, unsigned int *preserved, unsigned int toreserve);

#if defined(IR_RESERVE_IMPLEMENT) || defined(IR_IMPLEMENT)
	#include "implementation/ir_reserve_implementation.h"
#endif

#endif