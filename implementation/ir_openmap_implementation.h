/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_OPENMAP_IMPLEMENTATION
#define IR_OPENMAP_IMPLEMENTATION

namespace ir
{
	unsigned int openmap_granularity = 0;
};

void *ir::openmap(MapCache *cache, unsigned int offset, unsigned int size, openmapmode mode)
{
	//If the global value is not initialized
	if (openmap_granularity == 0)
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		openmap_granularity = sysinfo.dwAllocationGranularity;
	}

	unsigned int lowlimit = offset & ~(openmap_granularity - 1);
	unsigned int highlimit = (mode == openmapmode::openmap_read) ? offset + size : (offset + size + openmap_granularity - 1) & ~(openmap_granularity - 1);

	//If everything ok -> do nothing
	if ((cache->lowlimit <= lowlimit) && (highlimit <= cache->highlimit) && (mode == cache->mapmode))
	{
		return (char*)cache->mapstart + (offset - cache->lowlimit);
	}

	//If mapped -> unmap
	if (cache->mapstart != nullptr)
	{
		UnmapViewOfFile(cache->mapstart);
		cache->lowlimit = 0;
		cache->highlimit = 0;
		cache->mapstart = nullptr;
	}

	//If mapping exists, but not the right mode or insufficient size -> destroy mapping
	if (((highlimit > cache->maxmapsize) || (cache->mapmode != mode)) && (cache->hmapping != NULL))
	{
		CloseHandle(cache->hmapping);
		cache->hmapping = NULL;
		cache->maxmapsize = 0;
		cache->mapmode = openmapmode::openmap_no;
	}

	//Choose constants or close
	DWORD protect;
	DWORD access;
	switch (mode)
	{
	case openmapmode::openmap_read:
		protect = PAGE_READONLY;
		access = FILE_MAP_READ;
		break;
	case openmapmode::openmap_write:
		protect = PAGE_READWRITE;
		access = FILE_MAP_WRITE;
		break;
	case openmapmode::openmap_readwrite:
		protect = PAGE_READWRITE;
		access = FILE_MAP_READ | FILE_MAP_WRITE;
		break;
	default:
		return nullptr;
	}

	//If mapping does not exist -> create mapping
	//If success -> note mapmode and maxmapsize
	//If fail -> fail
	if (cache->hmapping == NULL)
	{
		cache->hmapping = CreateFileMapping(cache->hfile, nullptr, protect, 0, highlimit, nullptr);
		if (cache->hmapping == NULL) return nullptr;
		cache->mapmode = mode;
		cache->maxmapsize = highlimit;
	}

	//Try to get offset
	//If success -> note lowlimit and highlimit
	//If fail -> fail
	cache->mapstart = MapViewOfFile(cache->hmapping, access, 0, lowlimit, highlimit - lowlimit);
	if (cache->mapstart == nullptr) return nullptr;
	cache->lowlimit = lowlimit;
	cache->highlimit = highlimit;

	return (char*)cache->mapstart + (offset - lowlimit);
};
#endif	//#ifndef IR_OPENMAP_IMPLEMENTATION