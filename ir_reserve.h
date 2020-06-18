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
///@defgroup ir_reserve Reserve function
///@{

///Reserve data block
///@param pdata pointer to pointer on data block
///@param preserved Pointer to unsigned int that keeps the current size of data block
///@param toreserve	Required size of data block
///@return 1 on success, 0 on fail
unsigned char reserve(void **pdata, unsigned int *preserved, unsigned int toreserve);

#if (defined(IR_IMPLEMENT) || defined(IR_RESERVE_IMPLEMENT)) && !defined(IR_RESERVE_NOT_IMPLEMENT)
	#include <implementation/ir_reserve_implementation.h>
#endif

///@}
#endif