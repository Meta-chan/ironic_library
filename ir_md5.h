/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MD5
#define IR_MD5
	
#ifdef __cplusplus
	namespace ir
	{
	///@defgroup ir_md5 MD5 algorithm
	///@{

		///MD5 hash algorithm
		///@param[in]	data		Data to hash
		///@param[in]	bitlength	Length of data in bits
		///@param[out]	hash		Pointer to 16-bytes memory block to recieve the hash. May be <tt>int hash[4]</tt> or <tt>char hash[16]</tt> etc.
		void md5(const void *data, unsigned int bitlength, void *hash);
	
	///@}
	}
#else
	void ir_md5(const void *data, unsigned int bitlength, void *hash);
#endif

#if (defined(IR_MD5_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include <implementation/ir_md5_implementation.h>
#endif

#endif //#ifndef IR_MD5