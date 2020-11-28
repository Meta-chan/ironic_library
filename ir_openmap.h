/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_OPENMAP
#define IR_OPENMAP

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

	///ir::openmap operation mode
	enum class openmap_mode
	{
		read
	};

	///Cache structure needed by ir::openmap
	struct OpenmapCache
	{
		#ifdef _WIN32
			HANDLE hfile			= INVALID_HANDLE_VALUE;
			HANDLE hmapping			= NULL;					//indicated if mapping exists
			unsigned int maxmapsize	= 0;
			void *mapstart			= nullptr;				//indicated if address is got
		#else
			int filedes				= -1;
			void *mapstart			= MAP_FAILED;			//indicated if mapping exists
		#endif
		
		unsigned int lowlimit		= 0;
		unsigned int highlimit		= 0;
		
		void *emulatemem			= nullptr;
		unsigned int reserved		= 0;
	};

	///Opens file mappings. Is mapping fails for some reasons, ir::openmap tries to emulate it with simple file read/write.
	///@param cache		Cache needed for function.
	///@param file		Standard library file handle
	///@param offset	Offset of block you are interested on
	///@param size		Size of block you are interested on
	///@param mode		Operation mode
	///@return			Pointer to memory block of size @c size, where you car read/write dependent on @c mode
	void *openmap(OpenmapCache *cache, FILE *file, unsigned int offset, unsigned int size, openmap_mode mode);
	///Same as above, but with *nix file handle
	///@param filedes Native *nix file handle
	void *openmap(OpenmapCache *cache, int filedes, unsigned int offset, unsigned int size, openmap_mode mode);
	#ifdef _WIN32
		///Same as above, but with Windows file handle
		///@param hfile Native Windows file handle
		void *openmap(OpenmapCache *cache, HANDLE hfile, unsigned int offset, unsigned int size, openmap_mode mode);
	#endif
	///Close file mapping
	///@param cache Cache used by ir::openmap
	void closemap(OpenmapCache *cache);

///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_OPENMAP_IMPLEMENT)) && !defined(IR_OPENMAP_NOT_IMPLEMENT)
	#include <implementation/ir_openmap_implementation.h>
#endif

#endif	//#ifndef IR_OPENMAP
