/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_LIST
#define IR_LIST

#include <stddef.h>

namespace ir
{
///@addtogroup ir_container
///@{

	///Ironic library's list @n
	///It stores a pointer to memory that may be shared between several equal lists @n
	///Throws exceptions on errors
	///@tparam T Type of objects that the list contains
	template <class T> class List
	{
	protected:
		struct Element
		{
			Element *next;
			Element *previous;
			T element;
		};
		struct Head : Element
		{
			size_t size		= 0;
			size_t refcount	= 0;
		};
		
		mutable size_t _lastn	= 0;
		mutable Element *_lastp	= nullptr;
		Head *_head				= nullptr;	//_head is head, _head->previous is tail
		#ifdef _DEBUG
			Element *_debuglist	= nullptr;
		#endif
		static char const * const _out_of_range_message;
		Element *_find(size_t i) const;

	public:
		//Constructors:
		///Creates a list
		List()									noexcept;
		///Creates a list with given number of elements
		///@param newsize Number of elements
		List(size_t newsize);
		///Copies list
		///@param list List to be copied
		List(const List &list)					noexcept;


		//Non-constant access:
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
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &operator[](size_t i)	const;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &at(size_t i)			const;
		///Returns constant reference to first element
		inline const T &front()					const;
		///Returns constant reference to last element
		inline const T &back()					const;
		///Returns if list is empty
		
		//Maintenance:
		inline bool empty()						const noexcept;
		///Returns number of elements in list
		inline size_t size()					const noexcept;
		///Changes size of list
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear
		void resize(size_t newsize);
		///Puts an element on the end of the list
		///@param elem Element to be put on end
		void push_back(const T &elem);
		///Deletes an element from the end of the list
		void pop_back();
		///Inserts an element on the given position in the list. Elements after it are shifted right
		///@param i Index to place an element at
		///@param elem Element to insert
		void insert(size_t i, const T &elem);
		///Deletes an element on the given position in the list. Elements after it are shifted left
		///@param i Index of element to delete
		void erase(size_t i);
		///Assigns one list to another
		///@param list List to be assigned to
		const List &operator=(const List &list) noexcept;
		///Releases the list completely
		void clear()							noexcept;
		///Makes the list unique, not shared with other lists
		void detach();
		///Destroys the list
		~List()									noexcept;
	};
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_LIST_IMPLEMENT)) && !defined(IR_LIST_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_list_implementation.h>
#endif

#endif //#ifndef IR_LIST