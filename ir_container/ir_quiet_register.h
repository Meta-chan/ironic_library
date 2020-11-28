/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_REGISTER
#define IR_QUIET_REGISTER

#include <ir_syschar.h>
#include <ir_container/ir_quiet_vector.h>

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
	
	///Ironic library's quiet register. It is sorted ir::QuietVector with string as identifiers and binar search @n
	///It stores a pointer to memory that may be shared between several equal registers @n
	///Never throws exceptions on errors, but indicates failure through return values @n
	///If method that must return reference fails, it returns reference to internal static dummy
	///@tparam T Type of objects that the register contains
	///@tparam C Type of char, which strings serve as identifiers
	template <class T, class C = syschar> class QuietRegister : private QuietVector<RegisterElement<T, C>>
	{
	private:
		typedef QuietVector<RegisterElement<T, C>> super;

	protected:
		mutable size_t _last = 0;
		static size_t _strlen(const C *key) noexcept;
		static C *_strdup(const C *key, size_t length) noexcept;
		static int _strcmp(const C *key1, size_t keysize1, const C *key2, size_t keysize2) noexcept;
		bool _find(const C *key, size_t length, size_t *position) const noexcept;

	public:
		//Constuctors:
		///Creates a register
		QuietRegister()									noexcept;
		///Copies register
		///@param reg Register to be copied
		QuietRegister(QuietRegister &reg)				noexcept;
		
		//Non-constant element access:
		///Returns reference to element with specified identifier
		///@param key Null-terminated string identifer
		T &operator[](const C *key)						noexcept;
		///Returns reference to element with specified identifier
		///@param key Null-terminated string identifer
		T &at(const C *key)								noexcept;
		///Returns reference to element with specified identifier
		///@param key String identifer
		///@param length Length of the identifer in symbols
		T &at(const C *key, size_t length)				noexcept;
		///Assigns value to element with specified identifer
		///@param key Null-terminated string identifer
		///@param elem Value to be assigned
		bool set(const C *key, T elem)					noexcept;
		///Assigns value to element with specified identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		///@param elem Value to be assigned
		bool set(const C *key, size_t length, T elem)	noexcept;
		///Deletes an element with given identifer
		///@param key Null-terminated string identifer
		bool erase(const C *key)						noexcept;
		///Deletes an element with given identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		bool erase(const C *key, size_t length)			noexcept;
		///Returns reference to element on specified position, used for iterations over register
		///@param index Position of required element in register
		T &direct(size_t index)							noexcept;
		
		//Constant element access:
		///Returns constant reference to element with specified identifier
		///@param key Null-terminated string identifer
		const T &operator[](const C *key)				const noexcept;
		///Returns constant reference to element with specified identifier
		///@param key Null-terminated string identifer
		const T &at(const C *key)						const noexcept;
		///Returns constant reference to element with specified identifier
		///@param key String identifer
		///@param length Length of the identifer in symbols
		const T &at(const C *key, size_t length)		const noexcept;
		///Gets value of the element with specified identifer
		///@param key Null-terminated string identifer
		///@param elem Pointer to receive the value
		bool get(const C *key, T *elem)					const noexcept;
		///Gets value of the element with specified identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		///@param elem Pointer to receive the value
		bool get(const C *key, size_t length, T *elem)	const noexcept;
		///Checks if register containes an element with given identifer
		///@param key Null-terminated string identifer
		bool has(const C *key)							const noexcept;
		///Checks if register containes an element with given identifer
		///@param key String identifer
		///@param length Length of the identifer in symbols
		bool has(const C *key, size_t length)			const noexcept;
		///Returns constant reference to element on specified position, used for iterations over register
		///@param index Position of required element in register
		const T &direct(size_t index)					const noexcept;
		///Returns identfifer of element on specified position, used for iterations over register
		///@param index Position of required element in register
		const C *direct_key(size_t index)				const noexcept;
		///Returns length of identfifer of element on specified position, used for iterations over register
		///@param index Position of required element in register
		size_t direct_key_length(size_t index)			const noexcept;
		
		//Maintenance:
		using QuietVector<RegisterElement<T, C>>::empty;
		using QuietVector<RegisterElement<T, C>>::size;
		using QuietVector<RegisterElement<T, C>>::capacity;
		using QuietVector<RegisterElement<T, C>>::reserve;
		///Releases the register completely
		void clear()									noexcept;
		///Makes the register unique, not shared with other vectors
		bool detach()									noexcept;
		///Destroys the register
		~QuietRegister()								noexcept;
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_QUIET_REGISTER_IMPLEMENT)) && !defined(IR_QUIET_REGISTER_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_quiet_register_implementation.h>
#endif

#endif //#ifndef IR_QUIET_REGISTER