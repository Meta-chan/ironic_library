/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK
#define IR_BLOCK

namespace ir
{
///@defgroup ir_container Containers
///@{
	
	///Data block. Encapsulates raw pointer to data and it's size
	class Block
	{
	public:
		unsigned int size;	///< Size of data block
		void *data;			///< Raw pointer to the data
		
		///Creates a data block
		Block();
		///Creates a data block from pointer and size
		///@param size Size of data block
		///@param data Raw pointer to the data
		Block(unsigned int size, void *data);
	};

	///Constant data block. Encapsulates constant raw pointer to data and it's size
	class ConstBlock
	{
	public:
		unsigned int size;	///< Size of data block
		const void *data;	///< Constant raw pointer to the data
		
		///Creates a data block
		ConstBlock();
		///Creates a data block from pointer and size
		///@param size Size of data block
		///@param data Constant raw pointer to the data
		ConstBlock(unsigned int size, const void *data);
		///Creates a constant data block from data block
		///@param block Non-constant data block
		ConstBlock(Block block);
	};

///@}
};

#if (defined (IR_BLOCK_IMPLEMENT) || defined(IR_IMPLEMENT)) && !defined(IR_BLOCK_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_block_implementation.h>
#endif

#endif //#ifndef IR_BLOCK