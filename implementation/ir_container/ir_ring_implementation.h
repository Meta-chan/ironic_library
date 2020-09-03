/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RING_IMPLEMENTATION
#define IR_RING_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>

template<typename T> const char *ir::Ring<T>::_overflow_message = "ir::Ring is overflowed";
template<typename T> const char *ir::Ring<T>::_empty_message = "ir::Ring is empty";

template<typename T> ir::Ring<T>::Ring(size_t size)
{
	_data = (T*)malloc(size * sizeof(T));
	if (_data == nullptr) throw std::bad_alloc();
	_size = size;
};

template<typename T> typename ir::Ring<T>::Block ir::Ring<T>::write_direct(size_t count)
{
	if ((_size - _count) < count) throw std::runtime_error(_overflow_message);

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
};

template<typename T> typename ir::Ring<T>::Block ir::Ring<T>::read_direct(size_t count)
{
	if (count > _count) throw std::runtime_error(_empty_message);

	Block b;
	if (_head >= _tail)
	{
		//Normal situation, head is greater or equal to tail
		b.data[0] = _data + _tail;
		b.size[0] = count;
		b.data[1] = nullptr;
		b.size[1] = 0;
		_tail += count;
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
			_tail = b.size[1];
		}
		else
		{
			//Fits in one part
			b.data[0] = _data + _tail;
			b.size[0] = count;
			b.data[1] = nullptr;
			b.size[1] = 0;
			_tail += count;
			if (_tail == _size) _tail = 0;
		}
	}

	_count -= count;
	return b;
};

template<typename T> void ir::Ring<T>::write(const T *data, size_t count)
{
	Block b = write_direct(count);
	for (size_t i = 0; i < b.size[0]; i++) b.data[0][i] = data[i];
	for (size_t i = 0; i < b.size[1]; i++) b.data[1][i] = data[b.size[0] + i];
};

template<typename T> void ir::Ring<T>::read(T *data, size_t count)
{
	Block b = read_direct(count);
	for (size_t i = 0; i < b.size[0]; i++) data[i] = b.data[0][i];
	for (size_t i = 0; i < b.size[1]; i++) data[b.size[0] + i] = b.data[1][i];
};

template<typename T> void ir::Ring<T>::write(const T &elem)
{
	Block b = write_direct(1);
	b.data[0][0] = elem;
};

template<typename T> T ir::Ring<T>::read()
{
	Block b = read_direct(1);
	return b.data[0][0];
};

template<typename T> size_t ir::Ring<T>::count() const noexcept
{
	return _count;
};

template<typename T> size_t ir::Ring<T>::size() const noexcept
{
	return _size;
};

template<typename T> ir::Ring<T>::~Ring()
{
	if (_data != nullptr) free(_data);
};

#endif //#ifndef IR_RING_IMPLEMENTATION