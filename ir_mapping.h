/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MAPPING
#define IR_MAPPING

#include <ir_container/ir_quiet_vector.h>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <sys/mman.h>
#endif
#include <stdio.h>

namespace ir
{
///@defgroup ir_openmap Mapping function
///@{

	///Cache structure needed by ir::openmap
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
		Mapping() noexcept;
		///Opens file mappings. Is mapping fails for some reasons, ir::openmap tries to emulate it with simple file read/write.
		///@param cache		Cache needed for function.
		///@param file		Standard library file handle
		///@param offset	Offset of block you are interested on
		///@param size		Size of block you are interested on
		///@param mode		Operation mode
		///@return			Pointer to memory block of size @c size, where you car read/write dependent on @c mode
		void *map(FILE *file, size_t offset, size_t size, map_mode mode) noexcept;
		///Same as above, but with *nix file handle
		///@param filedes Native *nix file handle
		void *map(int filedes, size_t offset, size_t size, map_mode mode) noexcept;
		#ifdef _WIN32
			///Same as above, but with Windows file handle
			///@param hfile Native Windows file handle
			void *map(HANDLE hfile, size_t offset, size_t size, map_mode mode) noexcept;
		#endif
		///Closes file mapping
		void close() noexcept;
		///Destroys mapping object
		~Mapping() noexcept;
	};

///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_MAPPING_IMPLEMENT)) && !defined(IR_MAPPING_NOT_IMPLEMENT)
	#include <implementation/ir_mapping_implementation.h>
#endif

#endif	//#ifndef IR_MAPPING