/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_HASH_MAP_IMPLEMENTATION
#define IR_QUIET_HASH_MAP_IMPLEMENTATION

#include <new>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

template <class K, class V, class H> const typename ir::QuietHashMap<K, V, H>::Cell *ir::QuietHashMap<K, V, H>::_data() const noexcept
{
	if (_header == nullptr) return nullptr;
	else return (const Cell*)(_header + 1);
}

template <class K, class V, class H> typename ir::QuietHashMap<K, V, H>::Cell *ir::QuietHashMap<K, V, H>::_data() noexcept
{
	if (_header == nullptr) return nullptr;
	else return (Cell*)(_header + 1);
}

template <class K, class V, class H> bool ir::QuietHashMap<K, V, H>::_find(const K &key, size_t *position) const noexcept
{
	size_t s = size();
	if (s == 0) return false;

	if (_last < s && _data()[_last].state == state_exists && _data()[_last].key == key)
	{
		*position = _last;
		return true;
	}

	size_t search = H::hash(key) & (s - 1);
	size_t first_deleted = (size_t)-1;
	while (true)
	{
		if (_data()[search].state == state_exists)
		{
			*position = search;
			return true;
		}
		else if (_data()[search].state == state_deleted)
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

template<class K, class V, class H> ir::QuietHashMap<K, V, H>::QuietHashMap() noexcept
{
}

template<class K, class V, class H> ir::QuietHashMap<K, V, H>::QuietHashMap(const QuietHashMap &map) noexcept
{
	clear();
	_header = map._header;
	#ifdef _DEBUG
		_debugarray = _data();
	#endif
	if (_header != nullptr) _header->refcount++;
}

template<class K, class V, class H> const ir::QuietHashMap<K, V, H> &ir::QuietHashMap<K, V, H>::assign(const QuietHashMap<K, V, H> &map) noexcept
{
	if (_header != map._header)
	{
		clear();
		_header = map._header;
		#ifdef _DEBUG
			_debugarray = _data();
		#endif
		if (_header != nullptr) _header->refcount++;
	}
	return *this;
}

template<class K, class V, class H> const ir::QuietHashMap<K, V, H> &ir::QuietHashMap<K, V, H>::operator=(const QuietHashMap<K, V, H> &map) noexcept
{
	return assign(map);
}

template<class K, class V, class H> V &ir::QuietHashMap<K, V, H>::operator[](const K &key) noexcept
{
	return at(key);
}

template<class K, class V, class H> V &ir::QuietHashMap<K, V, H>::at(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	return _data()[position].value;
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::set(const K &key, const V &value) noexcept
{
	size_t position;
	if (_find(key, &position))
	{
		_data()[position].value = value;
		return true;
	}
	else if (size() == 0)
	{
		if (!resize(1)) return false;
		new(_data()) Cell;
		_data()[0].state = state_exists;
		_data()[0].key = key;
		_data()[0].value = value;
	}
	else
	{
		new(_data() + position) Cell;
		_data()[position].state = state_exists;
		_data()[position].key = key;
		_data()[position].value = value;
		return true;
	}
	if (2 * (_header->count + _header->delcount) >= _header->size)
	{
		if (!resize(2 * size())) return false;
	}
}

template<class K, class V, class H> void ir::QuietHashMap<K, V, H>::erase(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	_data()[position].~Cell();
	_data()[position].state = state_deleted;
}

template<class K, class V, class H> V &ir::QuietHashMap<K, V, H>::direct_value(size_t index) noexcept
{
	assert(index < size());
	assert(_data()[index].state == state_exists);
	return _data()[index].value;
}

template<class K, class V, class H> const V &ir::QuietHashMap<K, V, H>::operator[](const K &key) const noexcept
{
	return at(key);
}

template<class K, class V, class H> const V &ir::QuietHashMap<K, V, H>::at(const K &key) const noexcept
{
	size_t position;
	assert(_find(key, &position));
	return _data()[position].value;
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::has(const K &key) const noexcept
{
	size_t position;
	return _find(key, &position);
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::direct_exists(size_t index) const noexcept
{
	assert(index < size());
	return _data()[index].state == state_exists;
}

template<class K, class V, class H> const V &ir::QuietHashMap<K, V, H>::direct_value(size_t index) const noexcept
{
	assert(index < size());
	assert(_data()[index].state == state_exists);
	return _data()[index].value;
}

template<class K, class V, class H> const K &ir::QuietHashMap<K, V, H>::direct_key(size_t index) const noexcept
{
	assert(index < size());
	assert(_data()[index].state == state_exists);
	return _data()[index].key;
}

template<class K, class V, class H> size_t ir::QuietHashMap<K, V, H>::count() const noexcept
{
	return (_header == nullptr) ? 0 : _header->count;
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::empty() const noexcept
{
	return _header == nullptr || _header->count == 0;
}

template<class K, class V, class H> size_t ir::QuietHashMap<K, V, H>::size() const noexcept
{
	return (_header == nullptr) ? 0 : _header->size;
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::resize(size_t newsize) noexcept
{
	return detach(newsize);
}

template<class K, class V, class H> void ir::QuietHashMap<K, V, H>::clear() noexcept
{
	if (_header != nullptr)
	{
		_header->refcount--;
		if (_header->refcount == 0)
		{
			size_t s = size();
			Cell *d = _data();
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

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::detach() noexcept
{
	return detach(size());
}

template<class K, class V, class H> bool ir::QuietHashMap<K, V, H>::detach(size_t newsize) noexcept
{
	assert((newsize & (newsize - 1)) == 0);
	if (newsize < 2 * count()) return false;

	if (_header == nullptr)
	{
		_header = (Header*)malloc(sizeof(Header) + sizeof(Cell) * newsize);
		#ifdef _DEBUG
			_debugarray = _data();
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
		Cell *olddata = _data();
		if (!unique) clear();
		_header = (Header*)malloc(sizeof(Header) + sizeof(Cell) * newsize);
		#ifdef _DEBUG
			_debugarray = _data();
		#endif
		if (_header == nullptr) return false;
		memset(_header, 0, sizeof(Header) + sizeof(Cell) * newsize);
		_header->refcount = 1;
		_header->size = newsize;
		for (size_t i = 0; i < oldsize; i++) set(olddata[i].key, olddata[i].value);
		if (unique)
		{
			size_t s = size();
			Cell *d = _data();
			for (size_t i = 0; i < s; i++)
			{
				if (d[i].state == state_exists) d[i].~Cell();
			}
			free(((Header*)olddata) - 1);
		}
		return false;
	}
}

template<class K, class V, class H> ir::QuietHashMap<K, V, H>::~QuietHashMap() noexcept
{
	clear();
}

#endif //#ifndef IR_QUIET_HASH_MAP_IMPLEMENTATION