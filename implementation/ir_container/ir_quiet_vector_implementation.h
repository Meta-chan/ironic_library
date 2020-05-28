/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_VECTOR_IMPLEMENTATION
#define IR_QUIET_VECTOR_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>
#include <string.h>

template<class T> T ir::QuietVector<T>::_dummy;

template<class T> ir::QuietVector<T>::QuietVector() noexcept
{
};

template<class T> ir::QuietVector<T>::QuietVector(size_t newsize) noexcept
{
	resize(newsize);
};

template<class T> ir::QuietVector<T>::QuietVector(QuietVector &vector) noexcept
{
	clear();
	_header = vector._header;
	if (_header != nullptr) _header->refcount++;
};

template <class T> T *ir::QuietVector<T>::data() noexcept
{
	if (_header == nullptr) return nullptr;
	else return (T*)(_header + 1);
};

template<class T> T &ir::QuietVector<T>::operator[](size_t i) noexcept
{
	return at(i);
};

template<class T> T &ir::QuietVector<T>::at(size_t i) noexcept
{
	if (i >= size())
	{
		memset(&_dummy, 0, sizeof(T));
		return _dummy;
	}
	else return data()[i];
};

template<class T> T &ir::QuietVector<T>::front() noexcept
{
	return at(0);
};

template<class T> T &ir::QuietVector<T>::back() noexcept
{
	return at(size() - 1);
};

template<class T> bool ir::QuietVector<T>::set(size_t i, T elem) noexcept
{
	if (i >= size()) return false;
	data()[i] = elem;
	return true;
};

template <class T> const T *ir::QuietVector<T>::data() const noexcept
{
	if (size() == 0) return nullptr;
	else return (const T*)(_header + 1);
};

template<class T> const T &ir::QuietVector<T>::operator[](size_t i) const noexcept
{
	return at(i);
};

template<class T> const T &ir::QuietVector<T>::at(size_t i) const noexcept
{
	if (i >= size())
	{
		memset(&_dummy, 0, sizeof(T));
		return _dummy;
	}
	else return data()[i];
};

template<class T> const T &ir::QuietVector<T>::front() const noexcept
{
	return at(0);
};

template<class T> const T &ir::QuietVector<T>::back() const noexcept
{
	return at(size() - 1);
};

template<class T> bool ir::QuietVector<T>::get(size_t i, T *elem) const noexcept
{
	if (i >= size()) return false;
	*elem = data()[i];
	return true;
};

template<class T> bool ir::QuietVector<T>::empty() const noexcept
{
	return (size() == 0);
};

template<class T> size_t ir::QuietVector<T>::size() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->size;
};

template<class T> size_t ir::QuietVector<T>::capacity() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->capacity;
};

template<class T> bool ir::QuietVector<T>::resize(size_t newsize) noexcept
{
	if (!reserve(newsize)) return false;
	if (newsize > size()) memset(data() + size(), 0, (newsize - size()) * sizeof(T));
	_header->size = newsize;
	return true;
};

template<class T> bool ir::QuietVector<T>::reserve(size_t newcapacity) noexcept
{
	if (_header == nullptr)
	{
		_header = (Header*)malloc(sizeof(Header) + newcapacity * sizeof(T));
		if (_header == nullptr) return false;
		_header->refcount = 1;
		_header->size = 0;
		_header->capacity = newcapacity;
	}
	else if (_header->capacity < newcapacity)
	{
		_header = (Header*)realloc(_header, sizeof(Header) + newcapacity * sizeof(T));
		if (_header == nullptr) return false;
		_header->capacity = newcapacity;
	}
	return true;
};

template<class T> bool ir::QuietVector<T>::push_back(T elem) noexcept
{
	if (!resize(size() + 1)) return false;
	back() = elem;
	return true;
};

template<class T> bool ir::QuietVector<T>::pop_back() noexcept
{
	if (size() == 0) return false;
	else return resize(size() - 1);
};

template<class T> bool ir::QuietVector<T>::insert(size_t i, T elem) noexcept
{
	if (i > size()) return false;
	if (!resize(size() + 1)) return false;
	memcpy(data() + i + 1, data() + i, (size() - i - 1) * sizeof(T));
	data()[i] = elem;
	return true;
};

template<class T> bool ir::QuietVector<T>::erase(size_t i) noexcept
{
	if (i >= size()) return false;
	memcpy(data() + i, data() + i + 1, (size() - i - 1) * sizeof(T));
	return resize(size() - 1);
};

template<class T> void ir::QuietVector<T>::clear() noexcept
{
	if (_header != nullptr)
	{
		_header->refcount--;
		if (_header->refcount == 0) free(_header);
		_header = nullptr;
	}
};

template<class T> bool ir::QuietVector<T>::detach() noexcept
{
	if (_header != nullptr && _header->refcount != 1)
	{
		size_t oldsize = size();
		const void *olddata = data();
		clear();
		if (!resize(oldsize)) return false;
		memcpy(data(), olddata, oldsize);
	}
	return true;
};

template<class T> ir::QuietVector<T>::~QuietVector() noexcept
{
	clear();
};

#endif //#ifndef IR_QUIET_VECTOR_IMPLEMENTATION