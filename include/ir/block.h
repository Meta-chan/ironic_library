/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK
#define IR_BLOCK

namespace ir
{
	///@defgroup container Containers
	///@{

	///Block of data, encapsulates raw data and it's size
	class Block
	{
	private:
		size_t _size;		///< Size of data block
		const void *_data;	///< Constant raw pointer to the data

	public:
		///Creates empty data block
		Block()								noexcept;
		///Creates a data block from pointer and size
		///@param size Size of data block
		///@param data Constant raw pointer to the data
		Block(const void *data, size_t size)noexcept;
		///Returns raw data
		const void *data()					noexcept;
		///Returns size of raw data
		size_t size()						const noexcept;
		///Destroys data block
		~Block()							noexcept;
	};

	///@}
}

#ifndef IR_BLOCK_SOURCE
	#define IR_BLOCK_SOURCE
	#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_BLOCK) : !defined(IR_EXCLUDE_BLOCK)
		#if defined(IR_INCLUDE) && (IR_INCLUDE == 'a')
			#include "../../source/block.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_BLOCK