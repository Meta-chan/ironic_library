/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_VECTOR
#define IR_VECTOR

#include "quiet_vector.h"

namespace ir
{
///@addtogroup container Containers
///@{

	///Ironic library's vector @n
	///It stores a pointer to memory that may be shared between several equal vectors @n
	///`Vector` is a wrapper around `QuietVector`. It is designed to be friendly to programmer, which means:
	/// - `Vector` indicates leak of memory through `std::bad_alloc` exception.
	/// - `Vector` checks indexes with `assert` and may cause critical failure.
	/// - `Vector` makes vector unique on every non-constant operation.
	///@tparam T Type of objects that the vector contains
	template <class T> class Vector : protected QuietVector<T>
	{
	public:
		//Constructors:
		///Creates a vector
		Vector()										noexcept;
		///Creates a vector with given number of elements
		///@param newsize Number of elements
		Vector(size_t newsize);
		///Copies vector
		///@param vector vector to be copied
		Vector(const QuietVector<T> &vector)			noexcept;
		using QuietVector::assign;
		using QuietVector::operator=;
		
		//Non-constant access:
		///Returns raw pointer to data
		inline T *data();
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &operator[](size_t i);
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &at(size_t i);
		///Returns reference to first element
		inline T &front();
		///Returns reference to last element
		inline T &back();

		//Constant access:
		///Returns constant raw pointer to the data
		inline const T *data()							const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &operator[](size_t i)			const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &at(size_t i)					const noexcept;
		///Returns constant reference to first element
		inline const T &front()							const noexcept;
		///Returns constant reference to last element
		inline const T &back()							const noexcept;
		
		//Maintenance:
		using QuietVector<T>::size;
		using QuietVector<T>::empty;
		using QuietVector<T>::capacity;
		///Changes size of vector
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear
		inline void resize(size_t newsize);
		///Changes capacity of vector. It may be a good idea to reserve as many elements as you may need
		///@param newcapacity Required capacity
		inline void reserve(size_t newcapacity);
		///Puts an element on the end of the vector
		///@param elem Element to be put on end
		inline void push_back(const T &elem);
		///Deletes an element from the end of the vector
		inline void pop_back();
		///Inserts an element on the given position in the vector. Elements after it are shifted right
		///@param i Index to place an element at
		///@param elem Element to insert
		inline void insert(size_t i, const T &elem);
		///Deletes an element on the given position in the vector. Elements after it are shifted left
		///@param i Index of element to delete
		inline void erase(size_t i);
		using QuietVector<T>::clear;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_VECTOR) : !defined(IR_EXCLUDE_VECTOR)
	#ifndef IR_INCLUDE
		#ifndef IR_VECTOR_INLINE_SOURCE
			#define IR_VECTOR_INLINE_SOURCE
			#include "../../source/inline/vector.h"
		#endif
		#ifndef IR_VECTOR_TEMPLATE_SOURCE
			#define IR_VECTOR_TEMPLATE_SOURCE
			#include "../../source/template/vector.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_VECTOR_INLINE_SOURCE
			#define IR_VECTOR_INLINE_SOURCE
			#include "../../source/inline/vector.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_VECTOR_INLINE_SOURCE
			#define IR_VECTOR_INLINE_SOURCE
			#include "../../source/inline/vector.h"
		#endif
		#ifndef IR_VECTOR_TEMPLATE_SOURCE
			#define IR_VECTOR_TEMPLATE_SOURCE
			#include "../../source/template/vector.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_VECTOR