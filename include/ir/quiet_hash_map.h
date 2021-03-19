/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_HASH_MAP
#define IR_QUIET_HASH_MAP

namespace ir
{
///@addtogroup container Containers
///@{
	
	///Ironic library's quiet hash map @n
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

		bool _find(const K &key, size_t *position)				const noexcept;
		bool _detach()											noexcept;
		bool _detach(size_t newsize)							noexcept;

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
		inline V &at(const K &key)								noexcept;
		///Returns reference to element with specified identifier
		///@param key Element identifier
		inline V &operator[](const K &key)						noexcept;
		///Assigns value to element with specified identifier
		///@param key Element identifier
		///@param value Value to be assigned
		inline bool set(const K &key, const V &value)			noexcept;
		///Deletes an element with given identifier
		///@param key Element identifier
		inline bool erase(const K &key)							noexcept;
		///Returns reference to element on specified position, used for iterations over map
		///@param index Position of required element in map
		inline V &direct_value(size_t index)					noexcept;
		
		//Constant element access:
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &at(const K &key)						const noexcept;
		///Returns constant reference to element with specified identifier
		///@param key Element identifier
		inline const V &operator[](const K &key)				const noexcept;
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
		void clear()											noexcept;
		///Destroys map
		~QuietHashMap()											noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_QUIET_HASH_MAP) : !defined(IR_EXCLUDE_QUIET_HASH_MAP)
	#ifndef IR_INCLUDE
		#ifndef IR_QUIET_HASH_MAP_INLINE_SOURCE
			#define IR_QUIET_HASH_MAP_INLINE_SOURCE
			#include "../../source/inline/quiet_hash_map.h"
		#endif
		#ifndef IR_QUIET_HASH_MAP_TEMPLATE_SOURCE
			#define IR_QUIET_HASH_MAP_TEMPLATE_SOURCE
			#include "../../source/template/quiet_hash_map.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_QUIET_HASH_MAP_INLINE_SOURCE
			#define IR_QUIET_HASH_MAP_INLINE_SOURCE
			#include "../../source/inline/quiet_hash_map.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_QUIET_HASH_MAP_INLINE_SOURCE
			#define IR_QUIET_HASH_MAP_INLINE_SOURCE
			#include "../../source/inline/quiet_hash_map.h"
		#endif
		#ifndef IR_QUIET_HASH_MAP_TEMPLATE_SOURCE
			#define IR_QUIET_HASH_MAP_TEMPLATE_SOURCE
			#include "../../source/template/quiet_hash_map.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_QUIET_HASH_MAP