/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RING
#define IR_RING

#include <stddef.h>

namespace ir
{
///@addtogroup ir_container
///@{
	
	///Ironic library's FIFO ring buffer.
	///Throws exceptions when empty or overflowed
	template<typename T> class Ring
	{
	private:
		T *_data = nullptr;
		size_t _size = 0;
		size_t _head = 0;
		size_t _tail = 0;
		size_t _count = 0;
		static const char *_overflow_message;
		static const char *_empty_message;

	public:
		
		///Block of data. Your data is not guaranteed to be placed continuously,
		///it may be divided in two parts. @c Block represents these two parts.
		struct Block
		{
			///Parts of requested data.
			///First may be @c nullptr if requested @c count was zero.
			///Second may be @c nullptr if data if continuous
			T *data[2];
			///Sizes of data parts
			size_t size[2];
		};

		///Creates Ring
		///@param size Size of ring buffer
		Ring(size_t size);
		///Gives direct access to memory that should be written
		///@param count Requested number of elements
		Block write_direct(size_t count);
		///Gives direct access to memory that should be read
		///@param count Requested number of elements
		Block read_direct(size_t count);
		///Writes elements to Ring
		///@param data Elements to write
		///@param count Number of elements to write
		void write(const T *data, size_t count);
		///Reads elements from Ring
		///@param data Elements to read
		///@param count Number of elements to read
		void read(T *data, size_t count);
		///Writes one element to Ring
		///@param elem Element to write 
		void write(const T &elem);
		///Reads one element from Ring
		T read();
		///Returns number of currently stored elements
		size_t count() const noexcept;
		///Returns size of Ring
		size_t size() const noexcept;
		///Destroys Ring
		~Ring();
	};

///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_RING_IMPLEMENT)) && !defined(IR_RING_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_ring_implementation.h>
#endif

#endif //#ifndef IR_RING