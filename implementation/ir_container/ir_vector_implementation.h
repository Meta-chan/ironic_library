/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_VECTOR_IMPLEMENTATION
#define IR_VECTOR_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>
#include <string.h>

template<class T> char const * const ir::Vector<T>::_out_of_range_message = "ir::Vector out_of_range";

template<class T> ir::Vector<T>::Vector() noexcept
{
};

template<class T> ir::Vector<T>::Vector(size_t newsize)
{
	resize(newsize);
};

template<class T> ir::Vector<T>::Vector(Vector &vector) noexcept
{
	clear();
	_header = vector._header;
	if (_header != nullptr) _header->refcount++;
};

template <class T> T *ir::Vector<T>::data() noexcept
{
	if (_header == nullptr) return nullptr;
	else return (T*)(_header + 1);
};

template<class T> T &ir::Vector<T>::operator[](size_t i)
{
	return at(i);
};

template<class T> T &ir::Vector<T>::at(size_t i)
{
	if (i >= size()) throw std::out_of_range(_out_of_range_message);
	else return data()[i];
};

template<class T> T &ir::Vector<T>::front()
{
	return at(0);
};

template<class T> T &ir::Vector<T>::back()
{
	return at(size() - 1);
};

template <class T> const T *ir::Vector<T>::data() const noexcept
{
	if (size() == 0) return nullptr;
	else return (const T*)(_header + 1);
};

template<class T> const T &ir::Vector<T>::operator[](size_t i) const
{
	return at(i);
};

template<class T> const T &ir::Vector<T>::at(size_t i) const
{
	if (i >= size()) throw std::out_of_range(_out_of_range_message);
	else return data()[i];
};

template<class T> const T &ir::Vector<T>::front() const
{
	return at(0);
};

template<class T> const T &ir::Vector<T>::back() const
{
	return at(size() - 1);
};

template<class T> bool ir::Vector<T>::empty() const noexcept
{
	return (size() == 0);
};

template<class T> size_t ir::Vector<T>::size() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->size;
};

template<class T> size_t ir::Vector<T>::capacity() const noexcept
{
	if (_header == nullptr) return 0;
	else return _header->capacity;
};

template<class T> void ir::Vector<T>::resize(size_t newsize)
{
	reserve(newsize);
	if (newsize > size()) memset(data() + size(), 0, (newsize - size()) * sizeof(T));
	_header->size = newsize;
};

template<class T> void ir::Vector<T>::reserve(size_t newcapacity)
{
	if (_header == nullptr)
	{
		_header = (Header*) malloc(sizeof(Header) + newcapacity * sizeof(T));
		if (_header == nullptr) throw std::bad_alloc();
		_header->refcount = 1;
		_header->size = 0;
		_header->capacity = newcapacity;
	}
	else if (_header->capacity < newcapacity)
	{
		_header = (Header*)realloc(_header, sizeof(Header) + newcapacity * sizeof(T));
		if (_header == nullptr) throw std::bad_alloc();
		_header->capacity = newcapacity;
	}
};

template<class T> void ir::Vector<T>::push_back(T elem)
{
	resize(size() + 1);
	back() = elem;
};

template<class T> void ir::Vector<T>::pop_back()
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	resize(size() - 1);
};
	
template<class T> void ir::Vector<T>::insert(size_t i, T elem)
{
	if (i > size()) throw std::out_of_range(_out_of_range_message);
	resize(size() + 1);
	memcpy(data() + i + 1, data() + i, (size() - i - 1) * sizeof(T));
	*(data() + i) = elem;
};

template<class T> void ir::Vector<T>::erase(size_t i)
{
	if (i >= size()) throw std::out_of_range(_out_of_range_message);
	memcpy(data() + i, data() + i + 1, (size() - i - 1) * sizeof(T));
	resize(size() - 1);
};

template<class T> void ir::Vector<T>::clear() noexcept
{
	if (_header != nullptr)
	{
		_header->refcount--;
		if (_header->refcount == 0) free(_header);
		_header = nullptr;
	}
};

template<class T> void ir::Vector<T>::detach()
{
	if (_header != nullptr && _header->refcount != 1)
	{
		size_t oldsize = size();
		const void *olddata = data();
		clear();
		resize(oldsize);
		memcpy(data(), olddata, oldsize);
	}
};

template<class T> ir::Vector<T>::~Vector() noexcept
{
	clear();
};

#endif //#ifndef IR_VECTOR_IMPLEMENTATION