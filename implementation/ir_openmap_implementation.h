/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_OPENMAP_IMPLEMENTATION
#define IR_OPENMAP_IMPLEMENTATION

#ifdef _WIN32
	#include <io.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <stdlib.h>
#endif

namespace ir
{
	unsigned int _internal_openmap_pagesize = 0;
}

#ifdef _WIN32

void *ir::openmap(OpenmapCache *cache, HANDLE hfile, unsigned int offset, unsigned int size, openmap_mode mode)
{
	//If we need to recreate hmapping -> recreate hmapping (end delete mapstart, it will be also recreated)
	if (cache->hfile != hfile || cache->maxmapsize < offset + size || cache->hmapping == NULL)
	{
		if (cache->mapstart != nullptr) { UnmapViewOfFile(cache->mapstart); cache->mapstart = nullptr; }
		if (cache->hmapping != NULL) CloseHandle(cache->hmapping);
		cache->hfile = hfile;
		cache->maxmapsize = GetFileSize(cache->hfile, nullptr);
		cache->hmapping = CreateFileMappingW(cache->hfile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	}

	//If we have previous step done and need to recreate address -> recreate address
	if (cache->hmapping != NULL && (cache->mapstart == nullptr || offset <= cache->lowlimit || offset + size > cache->highlimit))
	{
		if (cache->mapstart != nullptr) UnmapViewOfFile(cache->mapstart);
		if (_internal_openmap_pagesize == 0)
		{
			SYSTEM_INFO sysinfo;
			GetSystemInfo(&sysinfo);
			_internal_openmap_pagesize = sysinfo.dwAllocationGranularity;
		}
		cache->lowlimit = offset & ~(_internal_openmap_pagesize - 1);
		cache->highlimit = (offset + size + _internal_openmap_pagesize - 1) & ~(_internal_openmap_pagesize - 1);
		if (cache->highlimit > cache->maxmapsize) cache->highlimit = cache->maxmapsize;
		cache->mapstart = MapViewOfFile(cache->hmapping, FILE_MAP_READ, 0, cache->lowlimit, cache->highlimit - cache->lowlimit);
	}
	
	//If we have previous step done, return pointer
	if (cache->mapstart != nullptr)
	{
		return (char*)cache->mapstart + (offset - cache->lowlimit);
	}
	//Otherwise we try to emulate
	else
	{
		if (cache->emulatemem == nullptr) cache->emulatemem = malloc(size);
		else if (cache->reserved < size)
		{
			cache->reserved = size;
			cache->emulatemem = realloc(cache->emulatemem, size);
		}
		if (cache->emulatemem == nullptr) return nullptr;

		if (SetFilePointer(cache->hfile, offset, nullptr, FILE_BEGIN) != offset) return nullptr;
		DWORD read;
		if (ReadFile(cache->hfile, cache->emulatemem, size, &read, nullptr) == FALSE || read < size) return nullptr;
		return cache->emulatemem;
	}
}

void *ir::openmap(OpenmapCache *cache, int filedes, unsigned int offset, unsigned int size, openmap_mode mode)
{
	return openmap(cache, (HANDLE)_get_osfhandle(filedes), offset, size, mode);
}

void *ir::openmap(OpenmapCache *cache, FILE *file, unsigned int offset, unsigned int size, openmap_mode mode)
{
	return openmap(cache, (HANDLE)_get_osfhandle(_fileno(file)), offset, size, mode);
}

void ir::closemap(OpenmapCache *cache)
{
	if (cache->mapstart != nullptr)
	{
		UnmapViewOfFile(cache->mapstart);
		cache->mapstart = nullptr;
	}

	if (cache->hmapping != NULL)
	{
		CloseHandle(cache->hmapping);
		cache->hmapping = NULL;
	}

	if (cache->emulatemem != nullptr)
	{
		free(cache->emulatemem);
		cache->emulatemem = nullptr;
	}
}

#else

void *ir::openmap(OpenmapCache *cache, int filedes, unsigned int offset, unsigned int size, openmap_mode mode)
{
	//If we need to recreate address -> recreate address
	if (cache->filedes != filedes || cache->mapstart == MAP_FAILED || offset <= cache->lowlimit || offset + size > cache->highlimit)
	{
		if (cache->mapstart != MAP_FAILED) munmap(cache->mapstart, cache->highlimit - cache->lowlimit);
		if (_internal_openmap_pagesize == 0) _internal_openmap_pagesize = sysconf(_SC_PAGESIZE);
		cache->filedes = filedes;
		cache->lowlimit = offset & ~(_internal_openmap_pagesize - 1);
		cache->highlimit = offset + size; //may be possible to optimize
		cache->mapstart = mmap(nullptr, cache->highlimit - cache->lowlimit, PROT_READ, MAP_PRIVATE, cache->filedes, cache->lowlimit);
	}

	//If we have previous step done, return pointer
	if (cache->mapstart != MAP_FAILED)
	{
		return (char*)cache->mapstart + (offset - cache->lowlimit);
	}
	//Otherwise we try to emulate
	else
	{
		if (cache->emulatemem == nullptr) cache->emulatemem = malloc(size);
		else if (cache->reserved < size)
		{
			cache->reserved = size;
			cache->emulatemem = realloc(cache->emulatemem, size);
		}
		if (cache->emulatemem == nullptr) return nullptr;
		
		if (lseek(cache->filedes, offset, SEEK_SET) != offset) return nullptr;
		if (read(cache->filedes, cache->emulatemem, size) < size) return nullptr;
		return cache->emulatemem;
	}
}

void *ir::openmap(OpenmapCache *cache, FILE *file, unsigned int offset, unsigned int size, openmap_mode mode)
{
	return openmap(cache, fileno(file), offset, size, mode);
}

void ir::closemap(OpenmapCache *cache)
{
	if (cache->mapstart != MAP_FAILED)
	{
		munmap(cache->mapstart, cache->highlimit - cache->lowlimit);
		cache->mapstart = MAP_FAILED;
	}

	if (cache->emulatemem != nullptr)
	{
		free(cache->emulatemem);
		cache->emulatemem = nullptr;
	}
}

#endif

#endif	//#ifndef IR_OPENMAP_IMPLEMENTATION
