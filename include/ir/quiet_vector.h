/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_VECTOR
#define IR_QUIET_VECTOR

#include <stddef.h>

namespace ir
{
///@addtogroup container Containers
///@{
		
	template <class T> class QuietVector
	{
	protected:
		struct Header
		{
			size_t size		= 0;
			size_t capacity = 0;
			size_t refcount	= 0;
		};
		Header *_header = nullptr;
		#ifdef _DEBUG
			T *_debugarray = nullptr;
		#endif
		bool _detach()											noexcept;
		bool _detach(size_t newcapacity)						noexcept;

	public:
		///Creates a vector
		QuietVector()											noexcept;
		///Creates a vector with given number of elements
		///@param newsize Number of elements
		QuietVector(size_t newsize)								noexcept;
		///Copies vector
		///@param vector Vector to be copied
		QuietVector(const QuietVector &vector)					noexcept;
		///Assigns one vector to another
		///@param vector vector to be assigned to
		const QuietVector &assign(const QuietVector &vector)	noexcept;
		///Assigns one vector to another
		///@param vector vector to be assigned to
		const QuietVector &operator=(const QuietVector &vector)	noexcept;
		
		///Returns raw pointer to data
		inline T *data()										noexcept;
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &operator[](size_t i)							noexcept;
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &at(size_t i)									noexcept;
		///Returns reference to first element
		inline T &front()										noexcept;
		///Returns reference to last element
		inline T &back()										noexcept;
		
		///Returns constant raw pointer to the data
		inline const T *data()									const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &operator[](size_t i)					const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &at(size_t i)							const noexcept;
		///Returns constant reference to first element
		inline const T &front()									const noexcept;
		///Returns constant reference to last element
		inline const T &back()									const noexcept;

		///Returns if vector is empty
		inline bool empty()										const noexcept;
		///Returns number of elements in vector
		inline size_t size()									const noexcept;
		///Returns number of currently allocated elements in vector
		inline size_t capacity()								const noexcept;
		///Changes size of vector
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear. May detach the vector
		bool resize(size_t newsize)								noexcept;
		///Changes capacity of vector. It may be a good idea to reserve as many elements as you may need. May detach the vector
		///@param newcapacity Required capacity
		bool reserve(size_t newcapacity)						noexcept;
		///Puts an element on the end of the vector. May detach the vector
		///@param elem Element to be put on end
		inline bool push_back(T elem)									noexcept;
		///Deletes an element from the end of the vector
		inline bool pop_back()											noexcept;
		///Inserts an element on the given position in the vector. Elements after it are shifted right. May detach the vector
		///@param i Index to place an element at
		///@param elem Element to insert
		inline bool insert(size_t i, T elem)							noexcept;
		///Deletes an element on the given position in the vector. Elements after it are shifted left
		///@param i Index of element to delete
		inline bool erase(size_t i)									noexcept;
		///Releases the vector completely
		void clear()											noexcept;
		///Destroys the vector
		~QuietVector()											noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_QUIET_VECTOR) : !defined(IR_EXCLUDE_QUIET_VECTOR)
	#ifndef IR_INCLUDE
		#ifndef IR_QUIET_VECTOR_INLINE_SOURCE
			#define IR_QUIET_VECTOR_INLINE_SOURCE
			#include "../../source/inline/quiet_vector.h"
		#endif
		#ifndef IR_QUIET_VECTOR_TEMPLATE_SOURCE
			#define IR_QUIET_VECTOR_TEMPLATE_SOURCE
			#include "../../source/template/quiet_vector.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_QUIET_VECTOR_INLINE_SOURCE
			#define IR_QUIET_VECTOR_INLINE_SOURCE
			#include "../../source/inline/quiet_vector.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_QUIET_VECTOR_INLINE_SOURCE
			#define IR_QUIET_VECTOR_INLINE_SOURCE
			#include "../../source/inline/quiet_vector.h"
		#endif
		#ifndef IR_QUIET_VECTOR_TEMPLATE_SOURCE
			#define IR_QUIET_VECTOR_TEMPLATE_SOURCE
			#include "../../source/template/quiet_vector.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_QUIET_VECTOR