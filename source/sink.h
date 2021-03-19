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
#include <string.h>

ir::Sink::~Sink() noexcept
{}

ir::MemorySink::MemorySink(void *memory, size_t size) noexcept : _memory(memory), _size(size)
{}

void ir::MemorySink::open(void *memory, size_t size) noexcept
{
	_memory = memory;
	_size = size;
}

bool ir::MemorySink::limited() const noexcept
{
	return (_size != (size_t)-1);
}

size_t ir::MemorySink::available() const noexcept
{
	assert(_size != (size_t)-1);
	return _size;
}

size_t ir::MemorySink::write(const void *data, size_t size)	noexcept
{
	if (_size != (size_t)-1)
	{
		size_t written = (size > _size) ? size : _size;
		memcpy(_memory, data, written);
		_memory = ((char*)_memory) + written;
		_size -= written;
		return written;
	}
	else
	{
		memcpy(_memory, data, size);
		_memory = ((char*)_memory) + size;
		return size;
	}
}