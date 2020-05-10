/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//openmap/closemap are functions to access files with map mechanism
//openmap does not open or close files itself, it must be done bu user

//IMPORTANT: Now only read mode is implemented. Sorry

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
	enum openmapmode
	{
		openmap_read
	};

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

	#ifdef _WIN32
		void *openmap(OpenmapCache *cache, HANDLE hfile, unsigned int offset, unsigned int size, openmapmode mode);
		void *openmap(OpenmapCache *cache, int filedes, unsigned int offset, unsigned int size, openmapmode mode);
		void *openmap(OpenmapCache *cache, FILE *file, unsigned int offset, unsigned int size, openmapmode mode);
	#else
		void *openmap(OpenmapCache *cache, int filedes, unsigned int offset, unsigned int size, openmapmode mode);
		void *openmap(OpenmapCache *cache, FILE *file, unsigned int offset, unsigned int size, openmapmode mode);
	#endif
		void closemap(OpenmapCache *cache);
};

#if (defined(IR_IMPLEMENT) || defined(IR_OPENMAP_IMPLEMENT)) && !defined(IR_OPENMAP_NOT_IMPLEMENT)
	#include <implementation/ir_openmap_implementation.h>
#endif

#endif	//#ifndef IR_OPENMAP
