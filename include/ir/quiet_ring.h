/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_RING
#define IR_QUIET_RING

#include <stddef.h>

namespace ir
{
///@addtogroup container Containers
///@{
	
	///Ironic library's FIFO ring buffer.
	template<class T> class QuietRing
	{
	private:
		T *_data = nullptr;
		size_t _size = 0;
		size_t _head = 0;
		size_t _tail = 0;
		size_t _count = 0;

	public:
		
		///Block of data. Your data is not guaranteed to be placed continuously,
		///it may be divided in two parts. `Block` represents these two parts.
		struct Block
		{
			///Parts of requested data.
			///First may be @c nullptr if requested @c count was zero.
			///Second may be @c nullptr if data if continuous
			T *data[2];
			///Sizes of data parts
			size_t size[2];
		};

		///Creates empty ring
		QuietRing()										noexcept;
		///Creates quiet ring
		///@param size Size of ring buffer
		QuietRing(size_t size)							noexcept;
		///Initializes quiet ring
		///@param size Size of ring buffer
		bool init(size_t size)							noexcept;
		///Returns if ring is properly initialized
		bool ok()										const noexcept;
		///Gives direct access to memory that should be written
		///@param count Requested number of elements
		Block direct_write(size_t count)				noexcept;
		///Writes elements to ring
		///@param count Number of elements
		///@param data Elements to write
		inline void write(size_t count, const T *data)	noexcept;
		///Writes one element to ring
		///@param data Element to write
		inline void write(const T &data)				noexcept;
		///Gives direct access to memory that should be read. Does not erase elements
		///@param count Requested number of elements
		Block direct_read(size_t count)					const noexcept;
		///Reads elements from ring
		///@param count Number of elements to read
		///@param data Buffer to read elements
		inline void read(size_t count, T *data)			const noexcept;
		///Reads one element from ring
		inline T read()									const noexcept;
		///Erases elements from the end of the ring
		///@param count Number of elements to be erased
		void erase(size_t count)						noexcept;
		
		///Returns number of currently stored elements
		inline size_t count()							const noexcept;
		///Returns size of Ring
		inline size_t size()							const noexcept;
		///Clears ring and frees resources
		void clear()									noexcept;
		///Destroys Ring
		~QuietRing()									noexcept;
	};

///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_QUIET_RING) : !defined(IR_EXCLUDE_QUIET_RING)
	#ifndef IR_INCLUDE
		#ifndef IR_QUIET_RING_INLINE_SOURCE
			#define IR_QUIET_RING_INLINE_SOURCE
			#include "../../source/inline/quiet_ring.h"
		#endif
		#ifndef IR_QUIET_RING_TEMPLATE_SOURCE
			#define IR_QUIET_RING_TEMPLATE_SOURCE
			#include "../../source/template/quiet_ring.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_QUIET_RING_INLINE_SOURCE
			#define IR_QUIET_RING_INLINE_SOURCE
			#include "../../source/inline/quiet_ring.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_QUIET_RING_INLINE_SOURCE
			#define IR_QUIET_RING_INLINE_SOURCE
			#include "../../source/inline/quiet_ring.h"
		#endif
		#ifndef IR_QUIET_RING_TEMPLATE_SOURCE
			#define IR_QUIET_RING_TEMPLATE_SOURCE
			#include "../../source/template/quiet_ring.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_QUIET_RING