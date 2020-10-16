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

	///Ironic library's vector @n
	///It stores a pointer to memory that may be shared between several equal vectors @n
	///Throws exceptions on errors
	///@tparam T Type of objects that the vector contains
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
		#ifdef _DEBUG
			T *_debugarray = nullptr;
		#endif
		static char const * const _out_of_range_message;

	public:
		//Constructors:
		///Creates a vector
		Vector()							noexcept;
		///Creates a vector with given number of elements
		///@param newsize Number of elements
		Vector(size_t newsize);
		///Copies vector
		///@param vector Vector to be copied
		Vector(const Vector &vector)		noexcept;

		//Non-constant access:
		///Returns raw pointer to data
		inline T *data()					noexcept;
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
		inline const T *data()				const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &operator[](size_t i)const;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &at(size_t i)		const;
		///Returns constant reference to first element
		inline const T &front()				const;
		///Returns constant reference to last element
		inline const T &back()				const;
		///Returns if vector is empty
		
		//Maintenance:
		///Returns whether vector is empty
		inline bool empty()					const noexcept;
		///Returns number of elements in vector
		inline size_t size()				const noexcept;
		///Returns number of currently allocated elements in vector
		inline size_t capacity()			const noexcept;
		///Changes size of vector
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear
		void resize(size_t newsize);
		///Changes capacity of vector. It may be a good idea to reserve as many elements as you may need
		///@param newcapacity Required capacity
		void reserve(size_t newcapacity);
		///Puts an element on the end of the vector
		///@param elem Element to be put on end
		void push_back(const T &elem);
		///Deletes an element from the end of the vector
		void pop_back();
		///Inserts an element on the given position in the vector. Elements after it are shifted right
		///@param i Index to place an element at
		///@param elem Element to insert
		void insert(size_t i, const T &elem);
		///Deletes an element on the given position in the vector. Elements after it are shifted left
		///@param i Index of element to delete
		void erase(size_t i);
		///Assigns one vector to another
		///@param vector Vector to be assigned to
		const Vector &operator=(const Vector &vector) noexcept;
		///Releases the vector completely
		void clear()						noexcept;
		///Makes the vector unique, not shared with other vectors
		void detach();
		///Makes the vector unique, not shared with other vectors and allocates more memory for it
		///@param newcapacity Required capacity
		void detach(size_t newcapacity);
		///Destroys the vector
		~Vector()							noexcept;
	};
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_VECTOR_IMPLEMENT)) && !defined(IR_VECTOR_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_vector_implementation.h>
#endif

#endif //#ifndef IR_VECTOR