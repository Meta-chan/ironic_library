/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_STRING
#define IR_STRING

#include <stddef.h>

namespace ir
{
///@addtogroup ir_container
///@{

	///Ironic library's string@n
	///The main feature is that the string can:
	///<ul>
	///<li>1) Store symbols in-place
	///<li>2) Store string in heap
	///<li>3) Store constant string
	///</ul>
	class String
	{
	protected:
		static char const * const _out_of_range_message;

		enum class Mode : unsigned char
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
			const char *data;
			mutable size_t size;
		};

		struct Stack
		{
			static const unsigned char max_size = sizeof(void*) + sizeof(size_t);
			Mode mode;
			unsigned char size;
			char data[max_size];
		};

		union
		{
			Heap _h;
			Stack _s;
			Constant _c;
		};

		#ifdef _DEBUG
			const char *_debug_string;
		#endif

	public:
		//Constructors
		///Creates string
		String()							noexcept;
		///Creates string of given size
		///@param size Size of the string
		///@param c Character to fill string with
		String(size_t size, char c);
		///Copies string
		///@param vector String to be copied
		String(const String &string)		noexcept;
		///Creates string from constant string
		///@warning
		///If the string is not constant and can be moved, freed or modified, use detach()!
		String(const char *string)			noexcept;

		//Non-constant access
		///Returns raw pointer to data
		char *data()						noexcept;
		///Returns reference to character with specified index
		///@param i Index of required character
		char &operator[](size_t i);
		///Returns reference to character with specified index
		///@param i Index of required character
		char &at(size_t i);
		///Returns reference to first character
		char &front();
		///Returns reference to last character
		char &back();

		//Constant access
		///Returns constant raw pointer to data
		const char *data()					const noexcept;
		///Returns constant reference to character with specified index
		///@param i Index of required character
		const char &operator[](size_t i)	const;
		///Returns constant reference to character with specified index
		///@param i Index of required character
		const char &at(size_t i)			const;
		///Returns constant reference to first character
		const char &front()					const;
		///Returns constant reference to last character
		const char &back()					const;

		//Maintenance
		///Returns whether string is empty
		bool empty()						const noexcept;
		///Returns number of characters in string
		size_t size()						const noexcept;
		///Returns number of currently allocated characters in string
		size_t capacity()					const noexcept;
		///Changes size of string
		///@param newsize Required size. Does not relieve memory completely, so consider using resize(0) instead of clear
		void resize(size_t newsize);
		///Changes capacity of string and makes it unique. It may be a good idea to reserve as many elements as you may need
		///@param newcapacity Required capacity
		void reserve(size_t newcapacity);
		///Releases string completely
		void clear()						noexcept;
		///Makes string unique
		void detach();
		///Destroys string
		~String()							noexcept;

		//Interactions
		///Assigns one string to another
		///@param string String to be assigned to
		const String &operator=(const String &string);
		///Performs concatenation with another string
		///@param string String to be added to the end
		const String &operator+=(const String &string);
	};

	//Non-member Interactions
	///Returns if two string are equal
	bool operator==(const String &a, const String &b)	noexcept;
	///Returns if two string are not equal
	bool operator!=(const String &a, const String &b)	noexcept;
	///Returns if one string is alphabetically less than another
	bool operator<(const String &a, const String &b)	noexcept;
	///Returns if one string is alphabetically less or equal to another
	bool operator<=(const String &a, const String &b)	noexcept;
	///Returns if one string is alphabetically greater than another
	bool operator>(const String &a, const String &b)	noexcept;
	///Returns if one string is alphabetically greater or equal to another
	bool operator>=(const String &a, const String &b)	noexcept;
	///Compares two strings alphabetically
	///@returns greater than zero if @c b is greater then @c a, less than zero if @c b is less then @c a, zero if equal
	int compare(const String &a, const String &b)		noexcept;
	///Performs concatenation of two strings
	const String operator+(const String &a, const String &b);

///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_STRING_IMPLEMENT)) && !defined(IR_STRING_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_string_implementation.h>
#endif

#endif //#ifndef IR_STRING