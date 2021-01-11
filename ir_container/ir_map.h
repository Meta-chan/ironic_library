/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MAP
#define IR_MAP

#include <ir_container/ir_quiet_map.h>

namespace ir
{
///@addtogroup ir_container
///@{
	
	///Ironic library's map @n
	///It is sorted ir::QuietVector with binary search @n
	///`Map` is a wrapper around `QuietMap`. It is designed to be friendly to programmer, which means:
	/// - `Map` indicates leak of memory through `std::bad_alloc` exception.
	/// - `Map` checks indexes with `assert` and may cause critical failure.
	/// - `Map` makes map unique on every non-constant operation.
		///@tparam K Key type
	///@tparam V Value type
	///@tparam C Comparator - a class with `static int compare(const K &k1, const K &k2) noexcept` member
	template <class K, class V, class C> class Map : private QuietMap<K, V, C>
	{
	private:
		typedef QuietMap<K, V, C> super;

	public:
		//Constuctors:
		///Creates a map
		Map()											noexcept;
		///Copies map
		///@param reg Map to be copied
		Map(const QuietMap &map)						noexcept;

		//Non-constant element access:
		///Returns reference to element with specified identifier
		///@param key Element identifier
		inline V &operator[](const K &key);
		///Returns reference to element with specified identifier
		///@param key Element identifier
		inline V &at(const K &key);
		///Assigns value to element with specified identifier
		///@param key Element identifier
		///@param elem Value to be assigned
		inline void set(const K &key, const V &elem);
		///Deletes an element with given identifier
		///@param key Element identifier
		inline void erase(const K &key);
		///Returns reference to element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline V &direct_value(size_t index);

		//Constant element access:
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &operator[](const K &key)		const noexcept;
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &at(const K &key)				const noexcept;
		///Returns constant reference to element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline const V &direct_value(size_t index)		const noexcept;

		//Maintenance:
		using super::has;
		using super::direct_key;
		using super::assign;
		using super::operator=;
		using super::empty;
		using super::size;
		using super::capacity;
		using super::reserve;
		using super::clear;
		using super::detach;
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_MAP_IMPLEMENT)) && !defined(IR_MAP_NOT_IMPLEMENT)
	#include "../implementation/ir_container/ir_map_implementation.h"
#endif

#endif //#ifndef IR_MAP