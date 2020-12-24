/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MAPPING_IMPLEMENTATION
#define IR_MAPPING_IMPLEMENTATION

#ifdef _WIN32
	#include <io.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <stdlib.h>
#endif

size_t ir::Mapping::_pagesize = 0;

#ifdef _WIN32

ir::Mapping::Mapping() noexcept
{
	if (_pagesize == 0)
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		_pagesize = sysinfo.dwAllocationGranularity;
	}
}

void *ir::Mapping::map(HANDLE hfile, size_t offset, size_t size, map_mode mode) noexcept
{
	//If we need to recreate hmapping -> recreate hmapping (end delete mapstart, it will be also recreated)
	if (_hfile != hfile || _maxmapsize < offset + size || _hmapping == NULL)
	{
		if (_mapstart != nullptr) { UnmapViewOfFile(_mapstart); _mapstart = nullptr; }
		if (_hmapping != NULL) CloseHandle(_hmapping);
		_hfile = hfile;
		_maxmapsize = GetFileSize(_hfile, nullptr);
		_hmapping = CreateFileMappingW(_hfile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	}

	//If we have previous step done and need to recreate address -> recreate address
	if (_hmapping != NULL && (_mapstart == nullptr || offset <= _lowlimit || offset + size > _highlimit))
	{
		if (_mapstart != nullptr) UnmapViewOfFile(_mapstart);
		_lowlimit = offset & ~(_pagesize - 1);
		_highlimit = (offset + size + _pagesize - 1) & ~(_pagesize - 1);
		if (_highlimit > _maxmapsize) _highlimit = _maxmapsize;
		_mapstart = MapViewOfFile(_hmapping, FILE_MAP_READ, 0, (unsigned int)_lowlimit, _highlimit - _lowlimit);
	}
	
	//If we have previous step done, return pointer
	if (_mapstart != nullptr)
	{
		return (char*)_mapstart + (offset - _lowlimit);
	}
	//Otherwise we try to emulate
	else
	{
		if (!_emulated.resize(size)) return nullptr;
		if (SetFilePointer(_hfile, (unsigned int)offset, nullptr, FILE_BEGIN) != offset) return nullptr;
		DWORD read;
		if (ReadFile(_hfile, &_emulated[0], (unsigned int)size, &read, nullptr) == FALSE || read < size) return nullptr;
		return &_emulated[0];
	}
}

void *ir::Mapping::map(int filedes, size_t offset, size_t size, map_mode mode) noexcept
{
	return map((HANDLE)_get_osfhandle(filedes), offset, size, mode);
}

void *ir::Mapping::map(FILE *file, size_t offset, size_t size, map_mode mode) noexcept
{
	return map((HANDLE)_get_osfhandle(_fileno(file)), offset, size, mode);
}

void ir::Mapping::close() noexcept
{
	if (_mapstart != nullptr)
	{
		UnmapViewOfFile(_mapstart);
		_mapstart = nullptr;
	}

	if (_hmapping != NULL)
	{
		CloseHandle(_hmapping);
		_hmapping = NULL;
	}

	_emulated.clear();
}

ir::Mapping::~Mapping() noexcept
{
	close();
}

#else

ir::Mapping::Mapping() noexcept
{
	if (_pagesize == 0) _pagesize = sysconf(_SC_PAGESIZE);
}

void *ir::Mapping::map(int filedes, size_t offset, size_t size, map_mode mode)
{
	//If we need to recreate address -> recreate address
	if (_filedes != filedes || _mapstart == MAP_FAILED || offset <= _lowlimit || offset + size > _highlimit)
	{
		if (_mapstart != MAP_FAILED) munmap(_mapstart, _highlimit - _lowlimit);
		_filedes = filedes;
		_lowlimit = offset & ~(_pagesize - 1);
		_highlimit = offset + size; //may be possible to optimize
		_mapstart = mmap(nullptr, _highlimit - _lowlimit, PROT_READ, MAP_PRIVATE, _filedes, _lowlimit);
	}

	//If we have previous step done, return pointer
	if (_mapstart != MAP_FAILED)
	{
		return (char*)_mapstart + (offset - _lowlimit);
	}
	//Otherwise we try to emulate
	else
	{
		if (!_emulated.resize(size))  return nullptr;
		if (lseek(_filedes, offset, SEEK_SET) != offset) return nullptr;
		if (read(_filedes, &_emulated[0], size) < size) return nullptr;
		return &_emulated[0];
	}
}

void *ir::Mapping::map(FILE *file, size_t offset, size_t size, map_mode mode) noexcept
{
	return openmap(fileno(file), offset, size, mode);
}

void ir::Mapping::close() noexcept
{
	if (_mapstart != MAP_FAILED)
	{
		munmap(_mapstart, _highlimit - _lowlimit);
		_mapstart = MAP_FAILED;
	}

	_emulated.clear();
}

it::Mapping::~Mapping() noexcept
{
	close();
}

#endif

#endif	//#ifndef IR_MAPPING_IMPLEMENTATION