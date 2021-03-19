/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <new>

template <class T>
inline T *ir::QuietVector<T>::data() noexcept
{
	if (empty()) return nullptr;
	if (_header->refcount > 1)
	{
		if (!_detach()) return nullptr;
	}
	return (T*)(_header + 1);
}

template<class T>
inline T &ir::QuietVector<T>::operator[](size_t i) noexcept
{
	return at(i);
}

template<class T>
inline T &ir::QuietVector<T>::at(size_t i) noexcept
{
	assert(i < size());
	if (_header->refcount > 1) assert(_detach());
	return data()[i];
}

template<class T>
inline T &ir::QuietVector<T>::front() noexcept
{
	return at(0);
}

template<class T>
inline T &ir::QuietVector<T>::back() noexcept
{
	return at(size() - 1);
}

template <class T>
inline const T *ir::QuietVector<T>::data() const noexcept
{
	if (empty()) return nullptr;
	return (const T*)(_header + 1);
}

template<class T>
inline const T &ir::QuietVector<T>::operator[](size_t i) const noexcept
{
	return at(i);
}

template<class T>
inline const T &ir::QuietVector<T>::at(size_t i) const noexcept
{
	assert(i < size());
	return data()[i];
}

template<class T>
inline const T &ir::QuietVector<T>::front() const noexcept
{
	return at(0);
}

template<class T>
inline const T &ir::QuietVector<T>::back() const noexcept
{
	return at(size() - 1);
}

template<class T>
inline bool ir::QuietVector<T>::empty() const noexcept
{
	return (size() == 0);
}

template<class T>
inline size_t ir::QuietVector<T>::size() const noexcept
{
	return (_header == nullptr) ? 0 : _header->size;
}

template<class T>
inline size_t ir::QuietVector<T>::capacity() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->capacity;
}

template<class T>
inline bool ir::QuietVector<T>::push_back(T elem) noexcept
{
	if (_header == nullptr || _header->refcount > 1 || _header->capacity < (size() + 1))
	{
		if (!_detach(size() + 1)) return false;
	}
	_header->size = size() + 1;
	new (&back()) T(elem);
	return true;
}

template<class T>
inline bool ir::QuietVector<T>::pop_back() noexcept
{
	assert(!empty());
	if (_header == nullptr || _header->refcount > 1)
	{
		if (!_detach()) return false;
	}
	back().~T();
	_header->size = size() - 1;
	return true;
}

template<class T>
inline bool ir::QuietVector<T>::insert(size_t i, T elem) noexcept
{
	assert(i <= size());
	if (_header == nullptr || _header->refcount > 1)
	{
		if (!_detach(size() + 1)) return false;
	}
	memcpy(data() + i + 1, data() + i, (size() - i) * sizeof(T));
	_header->size = size() + 1;
	new (&at(i)) T(elem);
	return true;
}

template<class T>
inline bool ir::QuietVector<T>::erase(size_t i) noexcept
{
	assert(i < size());
	if (_header == nullptr || _header->refcount > 1)
	{
		if (!_detach()) return false;
	}
	at(i).~T();
	memcpy(data() + i, data() + i + 1, (size() - i - 1) * sizeof(T));
	_header->size = size() - 1;
	return true;
}