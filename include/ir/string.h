/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_STRING
#define IR_STRING

#include "types.h"
#include <stddef.h>

namespace ir
{
///@addtogroup container Containers
///@{

	///Ironic library's string@n
	///The main feature is that the string can:
	/// - 1) Store symbols in-place
	/// - 2) Store string in heap
	/// - 3) Store constant string
	template<class T>
	class String
	{
	protected:
		enum class Mode : uint8
		{
			stack = 0,
			heap = 1,
			constant = 2
		};

		struct Heap
		{
			struct Header
			{
				size_t reserved;
				size_t refcount;
			};
			Mode mode;
			Header *data;
			size_t size;
		};

		struct Constant
		{
			static const size_t invalid_size = (size_t)-1;
			Mode mode;
			const T *data;
			mutable size_t size;
		};

		struct Stack
		{
			static const size_t max_size = (sizeof(void*) + sizeof(size_t)) / sizeof(T);
			Mode mode;
			unsigned char size;
			T data[max_size];
		};

		union
		{
			Stack _s;
			Heap _h;
			Constant _c;
		};

		#ifdef _DEBUG
			const T *_debug_string;
		#endif

		void _detach();

	public:
		//Constructors
		///Creates string
		String() noexcept;
		///Creates string of given size
		///@param size Size of the string
		///@param c Character to fill string with
		String(size_t size, T c);
		///Copies string
		///@param vector String to be copied
		String(const String &string) noexcept;
		///Creates string from constant string
		String(const T *string) noexcept;

		//Non-constant access
		///Returns raw pointer to data
		inline T *data() noexcept;
		///Returns reference to character with specified index
		///@param i Index of required character
		inline T &at(size_t i);
		///Returns reference to character with specified index
		///@param i Index of required character
		inline T &operator[](size_t i);
		///Returns reference to first character
		inline T &front();
		///Returns reference to last character
		inline T &back();
		///Inserts character to string
		///@param i Index to insert character
		///@param c Character to insert
		inline void insert(size_t i, T c);
		///Erases character from string
		///@param i Index of character to erase
		inline void erase(size_t i);
		///Puts character on the back of the string
		///@param c Character to put on the back
		inline void push_back(T c);
		///Removes characer from the back of the string
		inline void pop_back();

		//Constant access
		///Returns constant raw pointer to data
		inline const T *data() const noexcept;
		///Returns constant reference to character with specified index
		///@param i Index of required character
		inline const T &at(size_t i) const;
		///Returns constant reference to character with specified index
		///@param i Index of required character
		inline const T &operator[](size_t i) const;
		///Returns constant reference to first character
		inline const T &front() const;
		///Returns constant reference to last character
		inline const T &back() const;

		//Maintenance
		///Returns whether string is empty
		inline bool empty() const noexcept;
		///Returns number of characters in string
		inline size_t size() const noexcept;
		///Returns number of currently allocated characters in string
		inline size_t capacity() const noexcept;
		///Changes size of string
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear
		void resize(size_t newsize);
		///Changes capacity of string and makes it unique. It may be a good idea to reserve as many elements as you may need
		///@param newcapacity Required capacity
		void reserve(size_t newcapacity);
		///Releases string completely
		void clear() noexcept;
		///Transforms contsant strings to in-place string or heap string
		void save();
		///Destroys string
		~String() noexcept;

		//Interactions
		///Assigns one string to another
		///@param string String to be assigned to
		const String &assign(const String &string) noexcept;
		///Assigns one string to another
		///@param string String to be assigned to
		const String &operator=(const String &string) noexcept;
		///Performs concatenation with another string
		///@param string String to be added to the end
		const String &operator+=(const String &string);
	};

	//Non-member Interactions
	///Compares two strings alphabetically
	template<class T>
	int compare(const String<T> &a, const String<T> &b)				noexcept;
	///Compares two strings alphabetically
	template<class T>
	inline int compare(const String<T> &a, const T *b)				noexcept;
	///Compares two strings alphabetically
	template<class T>
	inline int compare(const T *a, const String<T> &b)				noexcept;
	///Returns if two string are equal
	template<class T>
	inline bool operator==(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if two string are equal
	template<class T>
	inline bool operator==(const String<T> &a, const T *b)			noexcept;
	///Returns if two string are equal
	template<class T>
	inline bool operator==(const T *a, const String<T> &b)			noexcept;
	///Returns if two string are not equal
	template<class T>
	inline bool operator!=(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if two string are not equal
	template<class T>
	inline bool operator!=(const T *a, const String<T> &b)			noexcept;
	///Returns if two string are not equal
	template<class T>
	inline bool operator!=(const String<T> &a, const T *b)			noexcept;
	///Returns if one string is alphabetically less than another
	template<class T>
	inline bool operator<(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if one string is alphabetically less than another
	template<class T>
	inline bool operator<(const String<T> &a, const T *b)			noexcept;
	///Returns if one string is alphabetically less than another
	template<class T>
	inline bool operator<(const T *a, const String<T> &b)			noexcept;
	///Returns if one string is alphabetically less or equal to another
	template<class T>
	inline bool operator<=(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if one string is alphabetically less or equal to another
	template<class T>
	inline bool operator<=(const String<T> &a, const T *b)			noexcept;
	///Returns if one string is alphabetically less or equal to another
	template<class T>
	inline bool operator<=(const T *a, const String<T> &b)			noexcept;
	///Returns if one string is alphabetically greater than another
	template<class T>
	inline bool operator>(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if one string is alphabetically greater than another
	template<class T>
	inline bool operator>(const String<T> &a, const T *b)			noexcept;
	///Returns if one string is alphabetically greater than another
	template<class T>
	inline bool operator>(const T *a, const String<T> &b)			noexcept;
	///Returns if one string is alphabetically greater or equal to another
	template<class T>
	inline bool operator>=(const String<T> &a, const String<T> &b)	noexcept;
	///Returns if one string is alphabetically greater or equal to another
	template<class T>
	inline bool operator>=(const String<T> &a, const T *b)			noexcept;
	///Returns if one string is alphabetically greater or equal to another
	template<class T>
	inline bool operator>=(const T *a, const String<T> &b)			noexcept;

	///Performs concatenation of two strings
	template<class T>
	const String<T> &operator+(const String<T> &a, const String<T> &b);
	///Performs concatenation of two strings
	template<class T>
	inline const String<T> &operator+(const String<T> &a, const T *b);
	///Performs concatenation of two strings
	template<class T>
	inline const String<T> &operator+(const T *a, const String<T> &b);

///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_STRING) : !defined(IR_EXCLUDE_STRING)
	#ifndef IR_INCLUDE
		#ifndef IR_STRING_INLINE_SOURCE
			#define IR_STRING_INLINE_SOURCE
			#include "../../source/inline/string.h"
		#endif
		#ifndef IR_STRING_TEMPLATE_SOURCE
			#define IR_STRING_TEMPLATE_SOURCE
			#include "../../source/template/string.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_STRING_INLINE_SOURCE
			#define IR_STRING_INLINE_SOURCE
			#include "../../source/inline/string.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_STRING_INLINE_SOURCE
			#define IR_STRING_INLINE_SOURCE
			#include "../../source/inline/string.h"
		#endif
		#ifndef IR_STRING_TEMPLATE_SOURCE
			#define IR_STRING_TEMPLATE_SOURCE
			#include "../../source/template/string.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_STRING