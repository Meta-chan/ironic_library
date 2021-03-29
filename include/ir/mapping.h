/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MAPPING
#define IR_MAPPING

#include "quiet_vector.h"
#ifdef _WIN32
	#include <Windows.h>
#else
	#include <sys/mman.h>
#endif
#include <stdio.h>
#include <stddef.h>

namespace ir
{
///@addtogroup file File utilities
///@{

	///Mapping utility
	class Mapping
	{
	public:
		///Mapping mode
		enum class map_mode
		{
			read
		};

	private:
		static size_t _pagesize;

		#ifdef _WIN32
			HANDLE _hfile			= INVALID_HANDLE_VALUE;
			HANDLE _hmapping		= NULL;					//indicated if mapping exists
			size_t _maxmapsize		= 0;
			void *_mapstart			= nullptr;				//indicated if address is got
		#else
			int _filedes			= -1;
			void *_mapstart			= MAP_FAILED;			//indicated if mapping exists
		#endif
		
		size_t _lowlimit			= 0;
		size_t _highlimit			= 0;
		
		QuietVector<char> _emulated;

	public:
		///Iinitalizes empty mapping object. Note that ir::Mapping can dynamically change files!
		Mapping()																noexcept;
		///Opens file mappings. Is mapping fails for some reasons, ir::openmap tries to emulate it with simple file read/write.
		///@param file		Standard library file handle
		///@param offset	Offset of block you are interested on
		///@param size		Size of block you are interested on
		///@param mode		Operation mode
		///@return			Pointer to memory block of size @c size, where you car read/write dependent on @c mode
		void *map(FILE *file, size_t offset, size_t size, map_mode mode)		noexcept;
		///Same as above, but with Unix file handle
		///@param filedes Native Unix file handle
		void *map(int filedes, size_t offset, size_t size, map_mode mode)		noexcept;
		#ifdef _WIN32
			///Same as above, but with Windows file handle
			///@param hfile Native Windows file handle
			void *map(HANDLE hfile, size_t offset, size_t size, map_mode mode)	noexcept;
		#endif
		///Closes file mapping
		void close()															noexcept;
		///Destroys mapping object
		~Mapping()																noexcept;
	};

///@}
}

#endif	//#ifndef IR_MAPPING

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_MAPPING) : !defined(IR_EXCLUDE_MAPPING)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_MAPPING_SOURCE
			#define IR_MAPPING_SOURCE
			#include "../../source/mapping.h"
		#endif
	#endif
#endif