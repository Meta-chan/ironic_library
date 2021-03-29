/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_LIST
#define IR_QUIET_LIST

#include <stddef.h>

namespace ir
{
///@addtogroup container Containers
///@{

	template <class T> class QuietList
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
		Element *_find(size_t i)							const noexcept;
		bool _detach()										noexcept;

	public:
		///Creates a list
		QuietList()											noexcept;
		///Creates a list with given number of elements
		///@param newsize Number of elements
		QuietList(size_t newsize)							noexcept;
		///Copies list
		///@param list List to be copied
		QuietList(const QuietList &list)					noexcept;
		///Assigns one list to another
		///@param list list to be assigned to
		const QuietList &assign(const QuietList &list)		noexcept;
		///Assigns one list to another
		///@param list list to be assigned to
		const QuietList &operator=(const QuietList &list)	noexcept;
		
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &operator[](size_t i)						noexcept;
		///Returns reference to element with specified index
		///@param i Index of required element
		inline T &at(size_t i)								noexcept;
		///Returns reference to first element
		inline T &front()									noexcept;
		///Returns reference to last element
		inline T &back()									noexcept;
		
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &operator[](size_t i)				const noexcept;
		///Returns constant reference to element with specified index
		///@param i Index of required element
		inline const T &at(size_t i)						const noexcept;
		///Returns constant reference to first element
		inline const T &front()								const noexcept;
		///Returns constant reference to last element
		inline const T &back()								const noexcept;

		///Returns if list is empty
		inline bool empty()									const noexcept;
		///Returns number of elements in list
		inline size_t size()								const noexcept;
		///Changes size of list
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear.
		bool resize(size_t newsize)							noexcept;
		///Puts an element on the end of the list.
		///@param elem Element to be put on end
		inline bool push_back(const T &elem)				noexcept;
		///Deletes an element from the end of the list
		inline bool pop_back()								noexcept;
		///Inserts an element on the given position in the list. Elements after it are shifted right.
		///@param i Index to place an element at
		///@param elem Element to insert
		inline bool insert(size_t i, const T &elem)			noexcept;
		///Deletes an element on the given position in the list. Elements after it are shifted left
		///@param i Index of element to delete
		inline bool erase(size_t i)							noexcept;
		///Releases the list completely
		void clear()										noexcept;
		///Destroys the list
		~QuietList()										noexcept;
	};
	
///@}
}

#endif //#ifndef IR_QUIET_LIST

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_QUIET_LIST) : !defined(IR_EXCLUDE_QUIET_LIST)
	#ifndef IR_INCLUDE
		#ifndef IR_QUIET_LIST_INLINE_SOURCE
			#define IR_QUIET_LIST_INLINE_SOURCE
			#include "../../source/inline/quiet_list.h"
		#endif
		#ifndef IR_QUIET_LIST_TEMPLATE_SOURCE
			#define IR_QUIET_LIST_TEMPLATE_SOURCE
			#include "../../source/template/quiet_list.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_QUIET_LIST_INLINE_SOURCE
			#define IR_QUIET_LIST_INLINE_SOURCE
			#include "../../source/inline/quiet_list.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_QUIET_LIST_INLINE_SOURCE
			#define IR_QUIET_LIST_INLINE_SOURCE
			#include "../../source/inline/quiet_list.h"
		#endif
		#ifndef IR_QUIET_LIST_TEMPLATE_SOURCE
			#define IR_QUIET_LIST_TEMPLATE_SOURCE
			#include "../../source/template/quiet_list.h"
		#endif
	#endif
#endif