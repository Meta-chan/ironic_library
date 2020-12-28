/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_VECTOR
#define IR_QUIET_VECTOR

#include <stddef.h>

namespace ir
{
///@addtogroup ir_container
///@{
		
	///Ironic library's quiet vector @n
	///It stores a pointer to memory that may be shared between several equal vectors @n
	///`QuietVector` is designed to be fast and reliable, which means:
	/// - `QuietVector` throws no exceptions.
	/// - `QuietVector` indicates leak of memory through return value of `resize`, `reserve` or `detach`.
	/// - `QuietVector` checks indexes with `assert` and may cause critical failure.
	/// - `QuietVector` does not check if vector is unique
	///@tparam T Type of objects that the vector contains
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
		bool push_back(T elem)									noexcept;
		///Deletes an element from the end of the vector
		bool pop_back()											noexcept;
		///Inserts an element on the given position in the vector. Elements after it are shifted right. May detach the vector
		///@param i Index to place an element at
		///@param elem Element to insert
		bool insert(size_t i, T elem)							noexcept;
		///Deletes an element on the given position in the vector. Elements after it are shifted left
		///@param i Index of element to delete
		bool erase(size_t i)									noexcept;
		///Releases the vector completely
		void clear()											noexcept;
		///Makes the vector unique, not shared with other vectors
		bool detach()											noexcept;
		///Makes the vector unique, not shared with other vectors and allocates more memory for it
		///@param newcapacity Required capacity
		bool detach(size_t newcapacity)							noexcept;
		///Destroys the vector
		~QuietVector()											noexcept;
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_QUIET_VECTOR_IMPLEMENT)) && !defined(IR_QUIET_VECTOR_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_quiet_vector_implementation.h>
#endif

#endif //#ifndef IR_QUIET_VECTOR