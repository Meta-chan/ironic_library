/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <assert.h>
#include <stdlib.h>
#include <new>

template<class T>
ir::QuietRing<T>::QuietRing() noexcept
{
}

template<class T>
ir::QuietRing<T>::QuietRing(size_t size) noexcept
{
	init(size);
}

template<class T>
bool ir::QuietRing<T>::init(size_t size) noexcept
{
	_data = (T*)malloc(size * sizeof(T));
	if (_data != nullptr) return false;
	_size = size;
	for (size_t i = 0; i < size; i++) new(&_data[i])T();
	return true;
}

template<class T>
bool ir::QuietRing<T>::ok() const noexcept
{
	return _data != nullptr;
}

template<class T>
typename ir::QuietRing<T>::Block ir::QuietRing<T>::direct_write(size_t count) noexcept
{
	assert(_data != nullptr);
	assert((_size - _count) >= count);

	Block b;
	if (_head >= _tail)
	{
		//Normal situation, head is greater or equal to tail
		if (count > (_size - _head))
		{
			//Does not fit in one part
			b.data[0] = _data + _head;
			b.size[0] = _size - _head;
			b.data[1] = _data;
			b.size[1] = count - b.size[0];
			_head = b.size[1];
		}
		else
		{
			//Fits in one part
			b.data[0] = _data + _head;
			b.size[0] = count;
			b.data[1] = nullptr;
			b.size[1] = 0;
			_head += count;
			if (_head == _size) _head = 0;
		}
	}
	else
	{
		//Inverse situation, tail is greater than head
		b.data[0] = _data + _head;
		b.size[0] = count;
		b.data[1] = nullptr;
		b.size[1] = 0;
		_head += count;
	}

	_count += count;
	return b;
}

template<class T>
typename ir::QuietRing<T>::Block ir::QuietRing<T>::direct_read(size_t count) const noexcept
{
	assert(_data != nullptr);
	assert(count <= _count);

	Block b;
	if (_head >= _tail)
	{
		//Normal situation, head is greater or equal to tail
		b.data[0] = _data + _tail;
		b.size[0] = count;
		b.data[1] = nullptr;
		b.size[1] = 0;
	}
	else
	{
		//Inverse situation, tail is greater than head
		if (count > (_size - _tail))
		{
			//Does not fit in one part
			b.data[0] = _data + _tail;
			b.size[0] = _size - _tail;
			b.data[1] = _data;
			b.size[1] = count - b.size[0];
		}
		else
		{
			//Fits in one part
			b.data[0] = _data + _tail;
			b.size[0] = count;
			b.data[1] = nullptr;
			b.size[1] = 0;
		}
	}

	return b;
}

template<class T>
void ir::QuietRing<T>::erase(size_t count) noexcept
{
	assert(_data != nullptr);
	assert(count <= _count);

	if (_head >= _tail)
	{
		//Normal situation, head is greater or equal to tail
		_tail += count;
	}
	else
	{
		//Inverse situation, tail is greater than head
		if (count > (_size - _tail))
		{
			//Does not fit in one part
			_tail = count - (_size - _tail);
		}
		else
		{
			//Fits in one part
			_tail += count;
			if (_tail == _size) _tail = 0;
		}
	}

	_count -= count;
}

template<class T>
void ir::QuietRing<T>::clear() noexcept
{
	if (_data != nullptr)
	{
		for (size_t i = 0; i < _size; i++) _data[i].~T();
		free(_data);
		_data = nullptr;
	}
	_size = 0;
	_head = 0;
	_tail = 0;
	_count = 0;
}

template<class T>
ir::QuietRing<T>::~QuietRing() noexcept
{
	clear();
}