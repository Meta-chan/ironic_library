/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_REGISTER
#define IR_REGISTER

#include <ir_syschar.h>
#include <ir_container/ir_vector.h>

namespace ir
{
///@addtogroup ir_container
///@{
	
	#ifndef DOXYGEN
		template <class T, class C> struct RegisterElement
		{
			C *key;
			size_t keysize;
			T data;
		};
	#endif
	
	///Ironic library's quiet register. It is sorted ir::Vector with string as identifiers and binar search @n
	///It stores a pointer to memory that may be shared between several equal registers @n
	///Throws exceptions on errors
	///@tparam T Type of objects that the register contains
	///@tparam C Type of char, which strings serve as identifiers
	template <class T, class C = syschar> class Register : private Vector<RegisterElement<T, C>>
	{
	private:
		typedef Vector<RegisterElement<T, C>> super;

	protected:
		mutable size_t _last = 0;
		static size_t _strlen(const C *key) noexcept;
		static C *_strdup(const C *key, size_t length);
		static int _strcmp(const C *key1, size_t keysize1, const C *key2, size_t keysize2) noexcept;
		bool _find(const C *key, size_t length, size_t *position) const noexcept;
		static const char * const _not_found_message;

	public:
		//Constuctors:
		///Creates a register
		Register()									noexcept;
		///Copies register
		///@param reg Register to be copied
		Register(Register &reg)				noexcept;
		
		//Non-constant element access:
		///Returns reference to element with specified identifier
		///@param key Null-terminated string identifer
		T &operator[](const C *key);
		///Returns reference to element with specified identifier
		///@param key Null-terminated string identifer
		T &at(const C *key);
		///Returns reference to element with specified identifier
		///@param key String identifer
		///@param length Length of the identifer in symbols
		T &at(const C *key, size_t length);
		///Creates element with specified identifer
		///@param key Null-terminated string identifer
		void create(const C *key);
		///Creates element with specified identifer
		///@param length Length of the identifer in symbols
		///@param key Null-terminated string identifer
		void create(const C *key, size_t length);
		///Assigns value to element with specified identifer
		///@param key Null-terminated string identifer
		///@param elem Value to be assigned
		void set(const C *key, const T &elem);
		///Assigns value to element with specified identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		///@param elem Value to be assigned
		void set(const C *key, size_t length, const T &elem);
		///Deletes an element with given identifer
		///@param key Null-terminated string identifer
		void erase(const C *key)						noexcept;
		///Deletes an element with given identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		void erase(const C *key, size_t length)			noexcept;
		///Returns reference to element on specified position, used for iterations over register
		///@param index Position of required element in register
		T &direct(size_t index)							noexcept;
		
		//Constant element access:
		///Returns constant reference to element with specified identifier
		///@param key Null-terminated string identifer
		const T &operator[](const C *key)				const;
		///Returns constant reference to element with specified identifier
		///@param key Null-terminated string identifer
		const T &at(const C *key)						const;
		///Returns constant reference to element with specified identifier
		///@param key String identifer
		///@param length Length of the identifer in symbols
		const T &at(const C *key, size_t length)		const;
		///Checks if register containes an element with given identifer
		///@param key Null-terminated string identifer
		bool has(const C *key)							const noexcept;
		///Checks if register containes an element with given identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		bool has(const C *key, size_t length)			const noexcept;
		///Returns constant reference to element on specified position, used for iterations over register
		///@param index Position of required element in register
		const T &direct(size_t index)					const;
		///Returns identfifer of element on specified position, used for iterations over register
		///@param index Position of required element in register
		const C *direct_key(size_t index)				const;
		///Returns length of identfifer of element on specified position, used for iterations over register
		///@param index Position of required element in register
		size_t direct_key_length(size_t index)			const;
		
		//Maintenance:
		using Vector<RegisterElement<T, C>>::empty;
		using Vector<RegisterElement<T, C>>::size;
		using Vector<RegisterElement<T, C>>::capacity;
		using Vector<RegisterElement<T, C>>::reserve;
		///Releases the register completely
		void clear()									noexcept;
		///Makes the register unique, not shared with other vectors
		void detach();
		///Destroys the register
		~Register()										noexcept;
	};
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_REGISTER_IMPLEMENT)) && !defined(IR_REGISTER_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_register_implementation.h>
#endif

#endif //#ifndef IR_REGISTER