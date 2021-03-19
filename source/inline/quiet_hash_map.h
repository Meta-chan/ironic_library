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

template<class K, class V, class H>
inline V &ir::QuietHashMap<K, V, H>::at(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	if (_header->refcount > 1) _detach();
	return ((Cell*)(_header + 1))[position].value;
}

template<class K, class V, class H>
inline V &ir::QuietHashMap<K, V, H>::operator[](const K &key) noexcept
{
	return at(key);
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::set(const K &key, const V &value) noexcept
{
	size_t position;
	if (_header != nullptr && _header->refcount > 1)
	{
		if (!_detach()) return false;
	}

	if (_find(key, &position))
	{
		((Cell*)(_header + 1))[position].value = value;
		return true;
	}
	
	if (size() == 0)
	{
		if (!resize(1)) return false;
		Cell *cell = (Cell*)(_header + 1);
		new(cell) Cell;
		cell->state = state_exists;
		cell->key = key;
		cell->value = value;
	}
	else
	{
		Cell *cell = ((Cell*)(_header + 1)) + position;
		new(cell) Cell;
		cell->state = state_exists;
		cell->key = key;
		cell->value = value;
	}

	if (2 * (_header->count + _header->delcount) >= _header->size)
	{
		if (!resize(2 * size())) return false;
	}
	return true;
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::erase(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	if (_header != nullptr && _header->refcount > 1)
	{
		if (!_detach()) return false;
	}
	Cell *cell = ((Cell*)(_header + 1)) + position;
	cell->~Cell();
	cell->state = state_deleted;
	return true;
}

template<class K, class V, class H>
inline V &ir::QuietHashMap<K, V, H>::direct_value(size_t index) noexcept
{
	assert(index < size());
	assert(((Cell*)(_header + 1))[index].state == state_exists);
	assert(_detach());
	return ((Cell*)(_header + 1))[index].value;
}

template<class K, class V, class H>
inline const V &ir::QuietHashMap<K, V, H>::at(const K &key) const noexcept
{
	size_t position;
	assert(_find(key, &position));
	return ((Cell*)(_header + 1))[position].value;
}

template<class K, class V, class H>
inline const V &ir::QuietHashMap<K, V, H>::operator[](const K &key) const noexcept
{
	return at(key);
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::has(const K &key) const noexcept
{
	size_t position;
	return _find(key, &position);
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::direct_exists(size_t index) const noexcept
{
	assert(index < size());
	return ((Cell*)(_header + 1))[index].state == state_exists;
}

template<class K, class V, class H>
inline const V &ir::QuietHashMap<K, V, H>::direct_value(size_t index) const noexcept
{
	assert(index < size());
	assert(((Cell*)(_header + 1))[index].state == state_exists);
	return ((Cell*)(_header + 1))[index].value;
}

template<class K, class V, class H>
inline const K &ir::QuietHashMap<K, V, H>::direct_key(size_t index) const noexcept
{
	assert(index < size());
	assert(((Cell*)(_header + 1))[index].state == state_exists);
	return ((Cell*)(_header + 1))[index].key;
}

template<class K, class V, class H>
inline size_t ir::QuietHashMap<K, V, H>::count() const noexcept
{
	return (_header == nullptr) ? 0 : _header->count;
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::empty() const noexcept
{
	return _header == nullptr || _header->count == 0;
}

template<class K, class V, class H>
inline size_t ir::QuietHashMap<K, V, H>::size() const noexcept
{
	return (_header == nullptr) ? 0 : _header->size;
}

template<class K, class V, class H>
inline bool ir::QuietHashMap<K, V, H>::resize(size_t newsize) noexcept
{
	return _detach(newsize);
}

template<class K, class V, class H>
inline void ir::QuietHashMap<K, V, H>::clear() noexcept
{
	if (_header != nullptr)
	{
		_header->refcount--;
		if (_header->refcount == 0)
		{
			size_t s = size();
			Cell *d = ((Cell*)(_header + 1));
			for (size_t i = 0; i < s; i++)
			{
				if (d[i].state == state_exists) d[i].~Cell();
			}
			free(_header);
		}
		_header = nullptr;
		#ifdef _DEBUG
			_debugarray = nullptr;
		#endif
	}
}