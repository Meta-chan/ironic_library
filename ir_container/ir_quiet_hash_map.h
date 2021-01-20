/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_HASH_MAP
#define IR_QUIET_HASH_MAP

namespace ir
{
///@addtogroup ir_container
///@{
	
	///Ironic library's quiet hash map @n
	///It is sorted ir::QuietVector with opened hashing @n
	///`QuietHashMap` is designed as RAM-only version of ir::S2STDatabase
	///`QuietHashMap` is designed to be fast and reliable, which means:
	/// - `QuietHashMap` throws no exceptions.
	/// - `QuietHashMap` indicates leak of memory through `false` as return value
	/// - `QuietHashMap` checks if elements exist with `assert` and may cause critical failure.
	/// - `QuietHashMap` does not check if map is unique
	///@tparam K Key type
	///@tparam V Value type
	///@tparam H Hasher - a class with `static size_t hash(const K &k) noexcept` member
	template <class K, class V, class H> class QuietHashMap
	{
	private:
		struct Cell
		{
			K key;
			V value;
			char state;
		};
		
		static const char state_not_exists	= 0;
		static const char state_deleted		= 1;
		static const char state_exists		= 2;
		
		struct Header
		{
			size_t size		= 0;
			size_t count	= 0;
			size_t delcount	= 0;
			size_t refcount	= 0;
		};
		
		mutable size_t _last = 0;
		Header *_header = nullptr;
		#ifdef _DEBUG
			Cell *_debugarray = nullptr;
		#endif

		inline Cell *_data()									noexcept;
		inline const Cell *_data()								const noexcept;
		bool _find(const K &key, size_t *position)				const noexcept;

	public:
		//Constuctors:
		///Creates a map
		QuietHashMap()											noexcept;
		///Copies map
		///@param reg Map to be copied
		QuietHashMap(const QuietHashMap &map)					noexcept;
		///Assigns one map to another
		///@param map Map to be assigned to
		const QuietHashMap &assign(const QuietHashMap &map)		noexcept;
		///Assigns one map to another
		///@param map Map to be assigned to
		const QuietHashMap &operator=(const QuietHashMap &map)	noexcept;

		//Non-constant element access:
		///Returns reference to element with specified identifier
		///@param key Element identifier
		inline V &operator[](const K &key)						noexcept;
		///Returns reference to element with specified identifier
		///@param key Element identifier
		inline V &at(const K &key)								noexcept;
		///Assigns value to element with specified identifier
		///@param key Element identifier
		///@param value Value to be assigned
		inline bool set(const K &key, const V &value)			noexcept;
		///Deletes an element with given identifier
		///@param key Element identifier
		inline void erase(const K &key)							noexcept;
		///Returns reference to element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline V &direct_value(size_t index)					noexcept;
		
		//Constant element access:
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &operator[](const K &key)				const noexcept;
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &at(const K &key)						const noexcept;
		///Checks if map contains an element with given identifier
		///@param key Element identifier
		inline bool has(const K &key)							const noexcept;
		///Reutns if element on specified position exists, used for iterations over map
		///@param index Position of required element in map
		inline bool direct_exists(size_t index)					const noexcept;
		///Returns constant reference to element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline const V &direct_value(size_t index)				const noexcept;
		///Returns identifier of element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline const K &direct_key(size_t index)				const noexcept;
		
		//Maintenance:
		///Returns element count
		inline size_t count()									const noexcept;
		///Returns if map contains no elements
		inline bool empty()										const noexcept;
		///Returns hash table size
		inline size_t size()									const noexcept;
		///Sets hash table size
		inline bool resize(size_t newsize)						noexcept;
		///Releases the map completely
		inline void clear()										noexcept;
		///Makes the map unique, not shared with other maps
		inline bool detach()									noexcept;
		///Makes the map unique, not shared with other map and sets table size for detached map
		///@param newsize Table size of detached map
		inline bool detach(size_t newsize)						noexcept;
		///Destroys map
		~QuietHashMap()											noexcept;
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_QUIET_HASH_MAP_IMPLEMENT)) && !defined(IR_QUIET_HASH_MAP_NOT_IMPLEMENT)
	#include "../implementation/ir_container/ir_quiet_hash_map_implementation.h"
#endif

#endif //#ifndef IR_QUIET_HASH_MAP