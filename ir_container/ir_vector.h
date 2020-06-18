/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_VECTOR
#define IR_VECTOR

#include <stddef.h>

namespace ir
{
///@addtogroup ir_container
///@{

	///Ironic library's vector
	///It stores a pointer to memory that may be shared between several equal vectors
	///Throws exceptions on errors
	template <class T> class Vector
	{
	protected:
		struct Header
		{
			size_t size		= 0;
			size_t capacity = 0;
			size_t refcount	= 0;
		};
		Header *_header = nullptr;
		static char const * const _out_of_range_message;

	public:
		///Creates a vector
		Vector()							noexcept;
		///Creates a vector with given number of elements
		///@param newsize Number of elements
		Vector(size_t newsize);
		///Copies vector
		///@param vector Vector to be copied
		Vector(Vector &vector)				noexcept;
		///Returns raw pointer to data
		T *data()							noexcept;
		///Returns reference to element with specified index
		///@param i Index of required element
		T &operator[](size_t i);
		///Returns reference to element with specified index
		///@param i Index of required element
		T &at(size_t i);
		///Returns reference to first element
		T &front();
		///Returns reference to last element
		T &back();
		///Returns constant raw pointer to the data
		const T *data()						const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		const T &operator[](size_t i)		const;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		const T &at(size_t i)				const;
		///Returns constant reference to first element
		const T &front()					const;
		///Returns constant reference to last element
		const T &back()						const;
		///Returns if vector is empty
		bool empty()						const noexcept;
		///Returns number of elements in vector
		size_t size()						const noexcept;
		///Returns number of currently allocated elements in vector
		size_t capacity()					const noexcept;
		///Changes size of vector
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear. May detach the vector
		void resize(size_t newsize);
		///Changes capacity of vector. It may be a good idea to reserve as many elements as you may need. May detach the vector
		///@param newcapacity Required capacity
		void reserve(size_t newcapacity);
		///Puts an element on the end of the vector. May detach the vector
		///@param elem Element to be put on end
		void push_back(T elem);
		///Deletes an element from the end of the vector
		void pop_back();
		///Inserts an element on the given position in the vector. Elements after it are shifted right. May detach the vector
		///@param i Index to place an element at
		///@param elem Element to insert
		void insert(size_t i, T elem);
		///Deletes an element on the given position in the vector. Elements after it are shifted left
		///@param i Index of element to delete
		void erase(size_t i);
		///Releases the vector completely
		void clear()						noexcept;
		///Makes the vector unique, not shared with other vectors
		void detach();
		///Destroys the vector
		~Vector()							noexcept;
	};
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_VECTOR_IMPLEMENT)) && !defined(IR_VECTOR_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_vector_implementation.h>
#endif

#endif //#ifndef IR_VECTOR