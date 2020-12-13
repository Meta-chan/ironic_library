/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_VECTOR_IMPLEMENTATION
#define IR_VECTOR_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

template<class T> ir::Vector<T>::Vector() noexcept
{
}

template<class T> ir::Vector<T>::Vector(size_t newsize)
{
	resize(newsize);
}

template<class T> ir::Vector<T>::Vector(const Vector &vector) noexcept
{
	clear();
	_header = vector._header;
	#ifdef _DEBUG
		_debugarray = _header != nullptr ? (T*)(_header + 1) : nullptr;
	#endif
	if (_header != nullptr) _header->refcount++;
}

template<class T> const ir::Vector<T> &ir::Vector<T>::assign(const Vector<T> &vector) noexcept
{
	if (_header != vector._header)
	{
		clear();
		_header = vector._header;
		#ifdef _DEBUG
			_debugarray = _header != nullptr ? (T*)(_header + 1) : nullptr;
		#endif
		if (_header != nullptr) _header->refcount++;
	}
	return *this;
}

template<class T> const ir::Vector<T> &ir::Vector<T>::operator=(const Vector<T> &vector) noexcept
{
	return assign(vector);
}

template <class T> T *ir::Vector<T>::data()
{
	if (empty()) return nullptr;
	detach();
	return (T*)(_header + 1);
}

template<class T> T &ir::Vector<T>::operator[](size_t i)
{
	return at(i);
}

template<class T> T &ir::Vector<T>::at(size_t i)
{
	assert(i < size());
	return data()[i];
}

template<class T> T &ir::Vector<T>::front()
{
	return at(0);
}

template<class T> T &ir::Vector<T>::back()
{
	return at(size() - 1);
}

template <class T> const T *ir::Vector<T>::data() const noexcept
{
	if (empty()) return nullptr;
	return (T*)(_header + 1);
}

template<class T> const T &ir::Vector<T>::operator[](size_t i) const noexcept
{
	return at(i);
}

template<class T> const T &ir::Vector<T>::at(size_t i) const noexcept
{
	assert(i < size());
	return data()[i];
}

template<class T> const T &ir::Vector<T>::front() const noexcept
{
	return at(0);
}

template<class T> const T &ir::Vector<T>::back() const noexcept
{
	return at(size() - 1);
}

template<class T> size_t ir::Vector<T>::size() const noexcept
{
	return (_header == nullptr ? 0 : _header->size);
}

template<class T> bool ir::Vector<T>::empty() const noexcept
{
	return size() == 0;
}

template<class T> size_t ir::Vector<T>::capacity() const noexcept
{
	return (_header == nullptr ? 0 : _header->capacity);
}

template<class T> void ir::Vector<T>::resize(size_t newsize)
{
	detach(newsize);
	if (newsize > size())
	{
		_header->size = newsize;
		for (size_t i = size(); i < newsize; i++)
		{
			memset(&at(i), 0, sizeof(T));
			new(&at(i)) T();
		}
	}
	else if (newsize < size())
	{
		for (size_t i = newsize; i < size(); i++) at(i).~T();
		_header->size = newsize;
	}
}

template<class T> void ir::Vector<T>::reserve(size_t newcapacity)
{
	//reserve is a light verson of detach(size_t), that does not care about refcount
	if (_header == nullptr)
	{
		_header = (Header*) malloc(sizeof(Header) + newcapacity * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header != nullptr ? (T*)(_header + 1) : nullptr;
		#endif
		if (_header == nullptr) throw std::bad_alloc();
		_header->refcount = 1;
		_header->size = 0;
		_header->capacity = newcapacity;
	}
	else if (_header->capacity < newcapacity)
	{
		if (newcapacity < 2 * _header->size) newcapacity = 2 * _header->size;
		_header = (Header*)realloc(_header, sizeof(Header) + newcapacity * sizeof(T));
		#ifdef _DEBUG
			_debugarray = _header != nullptr ? (T*)(_header + 1) : nullptr;
		#endif
		if (_header == nullptr) throw std::bad_alloc();
		_header->capacity = newcapacity;
	}
}

template<class T> void ir::Vector<T>::push_back(const T &elem)
{
	detach(size() + 1);
	_header->size = size() + 1;
	new (&back()) T(elem);
}

template<class T> void ir::Vector<T>::pop_back()
{
	assert(!empty());
	detach(size() - 1);
	back().~T();
	_header->size = size() - 1;
}
	
template<class T> void ir::Vector<T>::insert(size_t i, const T &elem)
{
	assert(i <= size());
	detach(size() + 1);
	memcpy(data() + i + 1, data() + i, (size() - i) * sizeof(T));
	_header->size = size() + 1;
	new (&at(i)) T(elem);
}

template<class T> void ir::Vector<T>::erase(size_t i)
{
	assert(i < size());
	detach(size() - 1);
	at(i).~T();
	memcpy(data() + i, data() + i + 1, (size() - i - 1) * sizeof(T));
	_header->size = size() - 1;
}

template<class T> void ir::Vector<T>::clear() noexcept
{
	if (_header != nullptr)
	{
		_header->refcount--;
		if (_header->refcount == 0)
		{
			for (size_t i = 0; i < size(); i++) at(i).~T();
			free(_header);
		}
		_header = nullptr;
		#ifdef _DEBUG
			_debugarray = nullptr;
		#endif
	}
}

template<class T> void ir::Vector<T>::detach()
{
	detach(size());
}

template<class T> void ir::Vector<T>::detach(size_t newcapacity)
{
	if (_header != nullptr && _header->refcount > 1)
	{
		if (size() > 0)
		{
			size_t oldsize = size();
			const T *olddata = (T*)(_header + 1);
			clear();
			reserve(newcapacity > oldsize ? newcapacity : oldsize);
			T *newdata = (T*)(_header + 1);
			for (size_t i = 0; i < oldsize; i++) new (&newdata[i]) T(olddata[i]);
			_header->refcount = 1;
			_header->size = oldsize;
		}
		else clear();
	}
}

template<class T> ir::Vector<T>::~Vector() noexcept
{
	clear();
}

#endif //#ifndef IR_VECTOR_IMPLEMENTATION