/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//openmap provides an access to files with a mapping mechanism.
//The advantage is that mapping objects are created/deleted not every time you access the file
//openmap does not open or close files. You must do it yourself by specifying hfile in MapCache
//openmap can be called several times without closing
//Use openmapmode::openmap_no to close the mapping

#ifndef IR_OPENMAP
#define IR_OPENMAP

#include <Windows.h>

namespace ir
{
	enum openmapmode : char
	{
		openmap_no			= 0,
		openmap_read		= 1,
		openmap_write		= 2,
		openmap_readwrite	= 3
	};

	struct MapCache
	{
		HANDLE hfile			= INVALID_HANDLE_VALUE;
		
		//Refer to hmapping
		HANDLE hmapping			= NULL;
		size_t maxmapsize		= 0;
		openmapmode mapmode		= openmapmode::openmap_read;

		//Refer to mapstart:
		void *mapstart			= nullptr;
		unsigned int lowlimit	= 0;
		unsigned int highlimit	= 0;
	};

	void *openmap(MapCache *cache, size_t offset, size_t size, openmapmode mode);
}

#if (defined(IR_OPENMAP_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include "implementation/ir_openmap_implementation.h"
#endif

#endif	//#ifndef IR_OPENMAP