/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <new>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

template <class K, class V, class H>
bool ir::QuietHashMap<K, V, H>::_find(const K &key, size_t *position) const noexcept
{
	size_t s = size();
	if (s == 0) return false;

	if (_last < s && ((Cell*)(_header + 1))[_last].state == state_exists && ((Cell*)(_header + 1))[_last].key == key)
	{
		*position = _last;
		return true;
	}

	size_t search = H::hash(key) & (s - 1);
	size_t first_deleted = (size_t)-1;
	while (true)
	{
		if (((Cell*)(_header + 1))[search].state == state_exists)
		{
			*position = search;
			return true;
		}
		else if (((Cell*)(_header + 1))[search].state == state_deleted)
		{
			if (first_deleted == (size_t)-1) first_deleted = search;
		}
		else
		{
			*position = (first_deleted == (size_t)-1) ? search : first_deleted;
			return false;
		}
		search++;
		if (search == s) search = 0;
	}
}

template<class K, class V, class H>
bool ir::QuietHashMap<K, V, H>::_detach() noexcept
{
	return _detach(size());
}

template<class K, class V, class H>
bool ir::QuietHashMap<K, V, H>::_detach(size_t newsize) noexcept
{
	assert((newsize & (newsize - 1)) == 0);
	if (newsize < 2 * count()) return false;

	if (_header == nullptr)
	{
		_header = (Header*)malloc(sizeof(Header) + sizeof(Cell) * newsize);
		#ifdef _DEBUG
			_debugarray = ((Cell*)(_header + 1));
		#endif
		if (_header == nullptr) return false;
		memset(_header, 0, sizeof(Header) + sizeof(Cell) * newsize);
		_header->refcount = 1;
		_header->size = newsize;
		return true;
	}
	else
	{
		bool unique = _header->refcount == 1;
		size_t oldsize = size();
		Cell *olddata = ((Cell*)(_header + 1));
		if (!unique) clear();
		_header = (Header*)malloc(sizeof(Header) + sizeof(Cell) * newsize);
		#ifdef _DEBUG
			_debugarray = ((Cell*)(_header + 1));
		#endif
		if (_header == nullptr) return false;
		memset(_header, 0, sizeof(Header) + sizeof(Cell) * newsize);
		_header->refcount = 1;
		_header->size = newsize;
		for (size_t i = 0; i < oldsize; i++) set(olddata[i].key, olddata[i].value);
		if (unique)
		{
			size_t s = size();
			Cell *d = ((Cell*)(_header + 1));
			for (size_t i = 0; i < s; i++)
			{
				if (d[i].state == state_exists) d[i].~Cell();
			}
			free(((Header*)olddata) - 1);
		}
		return false;
	}
}

template<class K, class V, class H>
ir::QuietHashMap<K, V, H>::QuietHashMap() noexcept
{
}

template<class K, class V, class H>
ir::QuietHashMap<K, V, H>::QuietHashMap(const QuietHashMap &map) noexcept
{
	clear();
	_header = map._header;
	#ifdef _DEBUG
		_debugarray = ((Cell*)(_header + 1));
	#endif
	if (_header != nullptr) _header->refcount++;
}

template<class K, class V, class H>
const ir::QuietHashMap<K, V, H> &ir::QuietHashMap<K, V, H>::assign(const QuietHashMap<K, V, H> &map) noexcept
{
	if (_header != map._header)
	{
		clear();
		_header = map._header;
		#ifdef _DEBUG
			_debugarray = ((Cell*)(_header + 1));
		#endif
		if (_header != nullptr) _header->refcount++;
	}
	return *this;
}

template<class K, class V, class H>
const ir::QuietHashMap<K, V, H> &ir::QuietHashMap<K, V, H>::operator=(const QuietHashMap<K, V, H> &map) noexcept
{
	return assign(map);
}

template<class K, class V, class H>
ir::QuietHashMap<K, V, H>::~QuietHashMap() noexcept
{
	clear();
}