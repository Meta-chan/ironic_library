/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_VECTOR_IMPLEMENTATION
#define IR_QUIET_VECTOR_IMPLEMENTATION

#include <new>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

template<class T> ir::QuietVector<T>::QuietVector() noexcept
{
}

template<class T> ir::QuietVector<T>::QuietVector(size_t newsize) noexcept
{
	resize(newsize);
}

template<class T> ir::QuietVector<T>::QuietVector(const QuietVector &vector) noexcept
{
	clear();
	_header = vector._header;
	#ifdef _DEBUG
		_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
	#endif
	if (_header != nullptr) _header->refcount++;
}

template<class T> const ir::QuietVector<T> &ir::QuietVector<T>::assign(const QuietVector<T> &vector) noexcept
{
	if (_header != vector._header)
	{
		clear();
		_header = vector._header;
		#ifdef _DEBUG
			_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
		#endif
		if (_header != nullptr) _header->refcount++;
	}
	return *this;
}

template<class T> const ir::QuietVector<T> &ir::QuietVector<T>::operator=(const QuietVector<T> &vector) noexcept
{
	return assign(vector);
}

template <class T> T *ir::QuietVector<T>::data() noexcept
{
	if (empty()) return nullptr;
	if (!detach()) return nullptr;
	return (T*)(_header + 1);
}

template<class T> T &ir::QuietVector<T>::operator[](size_t i) noexcept
{
	return at(i);
}

template<class T> T &ir::QuietVector<T>::at(size_t i) noexcept
{
	assert(i < size());
	return data()[i];
}

template<class T> T &ir::QuietVector<T>::front() noexcept
{
	return at(0);
}

template<class T> T &ir::QuietVector<T>::back() noexcept
{
	return at(size() - 1);
}

template <class T> const T *ir::QuietVector<T>::data() const noexcept
{
	if (empty()) return nullptr;
	return (const T*)(_header + 1);
}

template<class T> const T &ir::QuietVector<T>::operator[](size_t i) const noexcept
{
	return at(i);
}

template<class T> const T &ir::QuietVector<T>::at(size_t i) const noexcept
{
	assert(i < size());
	return data()[i];
}

template<class T> const T &ir::QuietVector<T>::front() const noexcept
{
	return at(0);
}

template<class T> const T &ir::QuietVector<T>::back() const noexcept
{
	return at(size() - 1);
}

template<class T> bool ir::QuietVector<T>::empty() const noexcept
{
	return (size() == 0);
}

template<class T> size_t ir::QuietVector<T>::size() const noexcept
{
	return (_header == nullptr) ? 0 : _header->size;
}

template<class T> size_t ir::QuietVector<T>::capacity() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->capacity;
}

template<class T> bool ir::QuietVector<T>::resize(size_t newsize) noexcept
{
	if (!detach(newsize)) return false;
	if (newsize > size())
	{
		size_t oldsize = size();
		_header->size = newsize;
		T *dat = data();
		memset(dat + oldsize, 0, (newsize - oldsize) * sizeof(T)); 
		for (size_t i = oldsize; i < newsize; i++) new(dat + i) T();
	}
	else if (newsize < size())
	{
		size_t oldsize = size();
		T *dat = data();
		for (size_t i = newsize; i < oldsize; i++) (dat + i)->~T();
		_header->size = newsize;
	}
	return true;
}

template<class T> bool ir::QuietVector<T>::reserve(size_t newcapacity) noexcept
{
	if (_header == nullptr)
	{
		_header = (Header*)malloc(sizeof(Header) + newcapacity * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
		#endif
		if (_header == nullptr) return false;
		_header->refcount = 1;
		_header->size = 0;
		_header->capacity = newcapacity;
	}
	else if (_header->capacity < newcapacity)
	{
		if (newcapacity < 2 * _header->size) newcapacity = 2 * _header->size;
		_header = (Header*)realloc(_header, sizeof(Header) + newcapacity * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
		#endif
		if (_header == nullptr) return false;
		_header->capacity = newcapacity;
	}
	return true;
}

template<class T> bool ir::QuietVector<T>::push_back(T elem) noexcept
{
	if (!detach(size() + 1)) return false;
	_header->size = size() + 1;
	new (&back()) T(elem);
	return true;
}

template<class T> bool ir::QuietVector<T>::pop_back() noexcept
{
	assert(!empty());
	if (!detach(size() - 1)) return false;
	back().~T();
	_header->size = size() - 1;
	return true;
}

template<class T> bool ir::QuietVector<T>::insert(size_t i, T elem) noexcept
{
	assert(i <= size());
	if (!detach(size() + 1)) return false;
	memcpy(data() + i + 1, data() + i, (size() - i) * sizeof(T));
	_header->size = size() + 1;
	new (&at(i)) T(elem);
	return true;
}

template<class T> bool ir::QuietVector<T>::erase(size_t i) noexcept
{
	assert(i < size());
	detach(size() - 1);
	at(i).~T();
	memcpy(data() + i, data() + i + 1, (size() - i - 1) * sizeof(T));
	_header->size = size() - 1;
	return true;
}

template<class T> void ir::QuietVector<T>::clear() noexcept
{
	if (_header != nullptr)
	{
		if (_header->refcount == 1)
		{
			size_t s = size();
			T *d = data();
			for (size_t i = 0; i < s; i++) d[i].~T();
			free(_header);
		}
		else _header->refcount--;
		_header = nullptr;
		#ifdef _DEBUG
			_debugarray = nullptr;
		#endif
	}
}

template<class T> bool ir::QuietVector<T>::detach() noexcept
{
	return detach(size());
}

template<class T> bool ir::QuietVector<T>::detach(size_t newcapacity) noexcept
{
	if (_header == nullptr)
	{
		return reserve(newcapacity);
	}
	else if (_header->refcount == 1)
	{
		return reserve(newcapacity);
	}
	else
	{
		size_t oldsize = size();
		const T *olddata = (T*)(_header + 1);
		clear();
		if (!reserve(newcapacity > oldsize ? newcapacity : oldsize)) return false;
		T *newdata = (T*)(_header + 1);
		for (size_t i = 0; i < oldsize; i++) new (&newdata[i]) T(olddata[i]);
		_header->refcount = 1;
		_header->size = oldsize;
	}
	return true;
}

template<class T> ir::QuietVector<T>::~QuietVector() noexcept
{
	clear();
}

#endif //#ifndef IR_QUIET_VECTOR_IMPLEMENTATION