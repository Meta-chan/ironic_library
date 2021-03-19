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

template<class T>
bool ir::QuietVector<T>::_detach() noexcept
{
	return _detach(size());
}

template<class T>
bool ir::QuietVector<T>::_detach(size_t newcapacity) noexcept
{
	if (_header == nullptr)
	{
		_header = (Header*) malloc(sizeof(Header) + newcapacity * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
		#endif
		if (_header == nullptr) return false;
		_header->size = 0;
		_header->capacity = newcapacity;
		_header->refcount = 1;
	}
	else if (_header->refcount == 1)
	{
		//TODO: Should allocate more memory then needed
		if (newcapacity < _header->capacity)
		{
			_header = (Header*)realloc(_header, sizeof(Header) + newcapacity * sizeof(T));
			#ifdef _DEBUG
				_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
			#endif
			if (_header == nullptr) return false;
		}
	}
	else
	{
		size_t oldsize = size();
		const T *olddata = (T*)(_header + 1);
		clear();
		if (newcapacity < oldsize) newcapacity = oldsize;
		_header = (Header*) malloc(sizeof(Header) + newcapacity  * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
		#endif
		if (_header == nullptr) return false;
		_header->size = oldsize;
		_header->capacity = newcapacity;
		_header->refcount = 1;
		T *newdata = (T*)(_header + 1);
		for (size_t i = 0; i < oldsize; i++) new (&newdata[i]) T(olddata[i]);
	}
	return true;
}

template<class T>
ir::QuietVector<T>::QuietVector() noexcept
{
}

template<class T>
ir::QuietVector<T>::QuietVector(size_t newsize) noexcept
{
	resize(newsize);
}

template<class T>
ir::QuietVector<T>::QuietVector(const QuietVector &vector) noexcept
{
	clear();
	_header = vector._header;
	#ifdef _DEBUG
		_debugarray = _header == nullptr ? nullptr : (T*)(_header + 1);
	#endif
	if (_header != nullptr) _header->refcount++;
}

template<class T>
const ir::QuietVector<T> &ir::QuietVector<T>::assign(const QuietVector<T> &vector) noexcept
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

template<class T>
const ir::QuietVector<T> &ir::QuietVector<T>::operator=(const QuietVector<T> &vector) noexcept
{
	return assign(vector);
}

template<class T>
bool ir::QuietVector<T>::resize(size_t newsize) noexcept
{
	if (_header == nullptr || _header->refcount > 1 || _header->capacity < newsize)
	{
		if (!_detach(newsize)) return false;
	}

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

template<class T>
bool ir::QuietVector<T>::reserve(size_t newcapacity) noexcept
{
	if (_header == nullptr || _header->capacity < newcapacity) return _detach(newcapacity);
	else return true;
}

template<class T>
void ir::QuietVector<T>::clear() noexcept
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

template<class T>
ir::QuietVector<T>::~QuietVector() noexcept
{
	clear();
}