/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

template <class T>
void ir::String<T>::_detach()
{
	if (_h.mode == Mode::heap)
	{
		if (_h.data->refcount > 1)
		{
			typename Heap::Header *new_data = (typename Heap::Header*)malloc(sizeof(typename Heap::Header) + (size() + 1) * sizeof(T));
			if (new_data == nullptr) throw std::bad_alloc();
			new_data->reserved = size();
			new_data->refcount = 1;
			_h.data->refcount--;
			memcpy(new_data + 1, _h.data + 1, (size() + 1) * sizeof(T));
			_h.data = new_data;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
	}
	else if (_c.mode == Mode::constant)
	{
		if (size() < Stack::max_size)
		{
			unsigned char old_size = (unsigned char)size();
			const T *old_data = _c.data;
			_s.mode = Mode::stack;
			_s.size = old_size;
			memcpy(_s.data, old_data, (old_size + 1) * sizeof(T));
		}
		else
		{
			typename Heap::Header *new_data = (typename Heap::Header*)malloc(sizeof(typename Heap::Header) + (size() + 1) * sizeof(T));
			if (new_data == nullptr) throw std::bad_alloc();
			new_data->reserved = size();
			new_data->refcount = 1;
			memcpy(new_data + 1, _c.data, (size() + 1) * sizeof(T));
			_h.mode = Mode::heap;
			_h.data = new_data;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
	}
}

template <class T>
ir::String<T>::String() noexcept
{
	memset(this, 0, sizeof(String<T>));
}

template <class T>
ir::String<T>::String(size_t size, T c)
{
	memset(this, 0, sizeof(String<T>));
	_h.data = (T*)malloc(sizeof(typename Heap::Header) + (size + 1) * sizeof(T));
	if (_h.data == nullptr) throw std::bad_alloc();
	#ifdef _DEBUG
		_debug_string = (T*)(_h.data + 1);
	#endif
	_h.mode = Mode::heap;
	_h.size = size;
	_h.data->reserved = size;
	_h.data->refcount = 1;
	T *dat = (T*)(_h.data + 1);
	for (size_t i = 0; i < size; i++) dat[i] = c;
	dat[size] = '\0';
}

template <class T>
ir::String<T>::String(const String &string) noexcept
{
	memcpy(this, &string, sizeof(String<T>));
	if (string._h.mode == Mode::heap) string._h.data->refcount++;
}

template <class T>
ir::String<T>::String(const T *string) noexcept
{
	memset(this, 0, sizeof(String<T>));
	if (string != nullptr)
	{
		_c.mode = Mode::constant;
		_c.size = Constant::invalid_size;
		_c.data = string;
	}
}

template <class T>
void ir::String<T>::resize(size_t newsize)
{
	reserve(newsize);
	if (_s.mode == Mode::stack)
	{
		_s.size = (unsigned char)newsize;
		_s.data[newsize] = '\0';
	}
	else
	{
		_h.size = newsize;
		((T*)(_h.data + 1))[newsize] = '\0';
	}
}

template <class T>
void ir::String<T>::reserve(size_t newcapacity)
{
	if (_s.mode == Mode::stack)
	{
		if (newcapacity > Stack::max_size)
		{
			//alloc & copy
			typename Heap::Header *new_data = (typename Heap::Header*)malloc(sizeof(typename Heap::Header) + newcapacity + 1);
			if (new_data == nullptr) throw std::bad_alloc();
			new_data->refcount = 1;
			new_data->reserved = newcapacity;
			memcpy(new_data + 1, _s.data, (_s.size + 1) * sizeof(T));
			_h.size = _s.size;
			_h.mode = Mode::heap;
			_h.data = new_data;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
	}
	else if (_h.mode == Mode::heap)
	{
		if (_h.data->refcount > 1)
		{
			//alloc & copy
			_h.data->refcount--;
			typename Heap::Header *new_data = (typename Heap::Header*)malloc(sizeof(typename Heap::Header) + (newcapacity + 1) * sizeof(T));
			if (new_data == nullptr) throw std::bad_alloc();
			new_data->refcount = 1;
			new_data->reserved = newcapacity;
			memcpy(new_data + 1, _h.data + 1, (_h.size + 1) * sizeof(T));
			_h.data = new_data;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
		else if (newcapacity > _h.data->reserved)
		{
			//realloc
			_h.data = (typename Heap::Header*)realloc(_h.data, sizeof(typename Heap::Header) + (newcapacity + 1) * sizeof(T));
			if (_h.data == nullptr) throw std::bad_alloc();
			_h.data->reserved = newcapacity;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
	}
	else
	{
		if (newcapacity < Stack::max_size)
		{
			//copy & switch to stack mode
			const T *old_data = _c.data;
			unsigned char old_size = (unsigned char)size();
			_s.mode = Mode::stack;
			memcpy(_s.data, old_data, (old_size + 1) * sizeof(T));
			_s.size = old_size;
		}
		else
		{
			//alloc & copy
			typename Heap::Header *new_data = (typename Heap::Header*)malloc(sizeof(typename Heap::Header) + (newcapacity + 1) * sizeof(T));
			if (new_data == nullptr) throw std::bad_alloc();
			new_data->refcount = 1;
			new_data->reserved = newcapacity;
			size_t old_size = size();
			memcpy(new_data + 1, _s.data, (old_size + 1) * sizeof(T));
			_h.mode = Mode::heap;
			_h.size = old_size;
			_h.data = new_data;
			#ifdef _DEBUG
				_debug_string = (T*)(_h.data + 1);
			#endif
		}
	}
}

template <class T>
void ir::String<T>::clear() noexcept
{
	if (_h.mode == Mode::heap && --_h.data->refcount == 0) free(_h.data);
	memset(this, 0, sizeof(String<T>));
}

template <class T>
ir::String<T>::~String() noexcept
{
	clear();
}

template <class T>
const ir::String<T> &ir::String<T>::assign(const String &string) noexcept
{
	if (this != &string)
	{
		if (_h.mode == Mode::heap && --_h.data->refcount == 0) free(_h.data);
		memcpy(this, &string, sizeof(String<T>));
		if (string._h.mode == Mode::heap) string._h.data->refcount++;
	}
	return *this;
}

template <class T>
const ir::String<T> &ir::String<T>::operator=(const String &string) noexcept
{
	return assign(string);
}

template <class T>
const ir::String<T> &ir::String<T>::operator+=(const String &string)
{
	size_t old_size = size();
	resize(old_size + string.size());
	memcpy(data() + old_size, string.data(), string.size() * sizeof(T));
	return *this;
}

template<class T>
int ir::compare(const String<T> &a, const String<T> &b) noexcept
{
	if (a.size() != b.size()) return (int)a.size() - (int)b.size();
	else if (sizeof(T) == 1) return memcmp((char*)a.data(), (char*)b.data(), a.size());
	else if (sizeof(T) == 2) return wmemcmp((wchar_t*)a.data(), (wchar_t*)b.data(), a.size());
	else for (size_t i = 0; i < a.size(); i++) { if (a[i] != b[i]) return (a[i] - b[i]); }
	return 0;
}

template <class T>
const ir::String<T> &ir::operator+(const String<T> &a, const String<T> &b)
{
	String<T> result;
	result.resize(a.size() + b.size());
	memcpy(result.data(), a.data(), a.size() * sizeof(T));
	memcpy(result.data() + a.size(), b.data(), b.size() * sizeof(T));
	return result;
}