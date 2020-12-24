/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK
#define IR_BLOCK

namespace ir
{
///@defgroup ir_container Containers
///@{
	
	///Data block is an interface to resiable block of data
	class Block
	{
	public:
		///Returns raw data
		virtual void *data()						noexcept = 0;
		///Returns size of raw data
		virtual size_t size()						const noexcept = 0;
		///Returns size of currently allocated memory
		virtual size_t capacity()					const noexcept = 0;
		///Resizes data block
		///@param newsize New size of data block
		virtual bool resize(size_t newsize)			noexcept = 0;
		///Allocates memory
		///@param newcapacity New capacity of data block
		virtual bool reserve(size_t newcapacity)	noexcept = 0;
		///Destroys data block
		virtual ~Block()							noexcept = 0;
	};

	///Constant data block is a non-resizable and constant block of data
	class ConstBlock
	{
	private:
		size_t _size;		///< Size of data block
		const void *_data;	///< Constant raw pointer to the data

	public:
		///Creates empty data block
		ConstBlock()								noexcept;
		///Creates a data block from pointer and size
		///@param size Size of data block
		///@param data Constant raw pointer to the data
		ConstBlock(const void *data, size_t size)	noexcept;
		///Returns raw data
		const void *data()							noexcept;
		///Returns size of raw data
		size_t size()								const noexcept;
		///Destroys data block
		~ConstBlock()								noexcept;
	};

///@}
}

#if (defined (IR_BLOCK_IMPLEMENT) || defined(IR_IMPLEMENT)) && !defined(IR_BLOCK_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_block_implementation.h>
#endif

#endif //#ifndef IR_BLOCK