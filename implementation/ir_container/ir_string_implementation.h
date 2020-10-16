/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_STRING_IMPLEMENTATION
#define IR_STRING_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>
#include <string.h>

char const * const ir::String::_out_of_range_message = "ir::String out of range";

ir::String::String() noexcept
{
	memset(this, 0, sizeof(String));
};

ir::String::String(size_t size, char c)
{
	memset(this, 0, sizeof(String));
	resize(size);
	memset(data(), c, size);
};

ir::String::String(const String &string) noexcept
{
	memcpy(this, &string, sizeof(String));
	if (string._h.mode == Mode::heap) string._h.data->refcount++;
};

ir::String::String(const char *string) noexcept
{
	if (string == nullptr) memset(this, 0, sizeof(String));
	else
	{
		_c.mode = Mode::constant;
		_c.size = Constant::invalid_size;
		_c.data = string;
	}
};

char *ir::String::data()  noexcept
{
	detach();
	if (_s.mode == Mode::stack) return _s.data;
	else return (char*)(_h.data + 1);
};

char &ir::String::operator[](size_t i) 
{
	detach();
	return at(i);
};

char &ir::String::at(size_t i)			
{
	detach();
	if (i > size()) throw std::out_of_range(_out_of_range_message);
	else return data()[i];
};

char &ir::String::front()					
{
	detach();
	if (size() != 0) throw std::out_of_range(_out_of_range_message);
	else return data()[0];
};

char &ir::String::back()					
{
	detach();
	if (size() != 0) throw std::out_of_range(_out_of_range_message);
	else return data()[size() - 1];
};

const char *ir::String::data() const noexcept
{
	if (_s.mode == Mode::stack) return _s.data;
	else if (_s.mode == Mode::heap) return (char*)(_h.data + 1);
	else return _c.data;
};

const char &ir::String::operator[](size_t i) const
{
	return at(i);
};

const char &ir::String::at(size_t i)			const
{
	if (i > size()) throw std::out_of_range(_out_of_range_message);
	else return data()[i];
};

const char &ir::String::front()					const
{
	if (size() != 0) throw std::out_of_range(_out_of_range_message);
	else return data()[0];
};

const char &ir::String::back()					const
{
	if (size() != 0) throw std::out_of_range(_out_of_range_message);
	else return data()[size() - 1];
};

bool ir::String::empty() const noexcept
{
	return (size() == 0);
};

size_t ir::String::size() const noexcept
{
	if (_s.mode == Mode::stack) return _s.size;
	else if (_h.mode == Mode::heap) return _h.size;
	else if (_c.size != Constant::invalid_size) return _c.size;
	else return (_c.size = strlen(_c.data));
};

size_t ir::String::capacity() const noexcept
{
	if (_s.mode == Mode::stack) return Stack::max_size;
	else if (_h.mode == Mode::heap) return _h.data->reserved;
	else return 0;
};

void ir::String::resize(size_t newsize)
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
		((char*)(_h.data + 1))[newsize] = '\0';
	}
};

void ir::String::reserve(size_t newcapacity)
{
	if (_s.mode == Mode::stack)
	{
		if (newcapacity >= Stack::max_size)
		{
			//alloc & copy
			Heap::Header *newdata = (Heap::Header*)malloc(sizeof(Heap::Header)+newcapacity+1);
			if (newdata == nullptr) throw std::bad_alloc();
			newdata->refcount = 1;
			newdata->reserved = newcapacity;
			memcpy(newdata + 1, _s.data, _s.size + 1);
			_h.mode = Mode::heap;
			_h.data = newdata;
		}
	}
	else if (_h.mode == Mode::heap)
	{
		if (_h.data->refcount > 1)
		{
			//alloc & copy
			_h.data->refcount--;
			Heap::Header *newdata = (Heap::Header*)malloc(sizeof(Heap::Header)+newcapacity+1);
			if (newdata == nullptr) throw std::bad_alloc();
			newdata->refcount = 1;
			newdata->reserved = newcapacity;
			memcpy(newdata + 1, _h.data + 1, _h.size + 1);
			_h.data = newdata;
		}
		else if (newcapacity > _h.data->reserved)
		{
			//realloc
			_h.data = (Heap::Header*)realloc(_h.data, sizeof(Heap::Header)+newcapacity+1);
			if (_h.data == nullptr) throw std::bad_alloc();
			_h.data->reserved = newcapacity;
		}
	}
	else
	{
		if (newcapacity < Stack::max_size)
		{
			//copy & switch to stack mode
			const char *constdata = _c.data;
			size_t constsize = (_c.size == Constant::invalid_size) ? strlen(_c.data) : _c.size;
			_s.mode = Mode::stack;
			memcpy(_s.data, constdata, constsize + 1);
		}
		else
		{
			//alloc & copy
			Heap::Header *newdata = (Heap::Header*)malloc(sizeof(Heap::Header)+newcapacity+1);
			if (newdata == nullptr) throw std::bad_alloc();
			newdata->refcount = 1;
			newdata->reserved = newcapacity;
			size_t constsize = (_c.size == Constant::invalid_size) ? strlen(_c.data) : _c.size;
			memcpy(newdata + 1, _s.data, constsize + 1);
			_h.mode = Mode::heap;
			_h.data = newdata;
		}
	}
};

void ir::String::clear() noexcept
{
	if (_h.mode == Mode::heap && --_h.data->refcount == 0) free(_h.data);
	memset(this, 0, sizeof(String));
};

void ir::String::detach()
{
	reserve(size());
};

ir::String::~String() noexcept
{
	clear();
};


const ir::String &ir::String::operator=(const String &string)
{
	if (_h.mode == Mode::heap && --_h.data->refcount == 0) free(_h.data);
	memcpy(this, &string, sizeof(String));
	if (string._h.mode == Mode::heap) string._h.data->refcount++;
	return *this;
};

const ir::String &ir::String::operator+=(const String &string)
{
	size_t oldsize = size();
	resize(oldsize + string.size());
	memcpy(data() + oldsize, string.data(), string.size());
	return *this;
};

bool ir::operator==(const String &a, const String &b) noexcept
{
	return (a.size() == b.size() && memcmp(a.data(), b.data(), a.size()) == 0);
};

bool ir::operator!=(const String &a, const String &b) noexcept
{
	return !(a == b);
};

bool ir::operator<(const String &a, const String &b) noexcept
{
	return (a.size() < b.size() || memcmp(a.data(), b.data(), a.size()) < 0);
};

bool ir::operator<=(const String &a, const String &b) noexcept
{
	return (a.size() <= b.size() || memcmp(a.data(), b.data(), a.size()) <= 0);
};

bool ir::operator>(const String &a, const String &b) noexcept
{
	return !(a <= b);
};

bool ir::operator>=(const String &a, const String &b) noexcept
{
	return !(a < b);
};

int ir::compare(const String &a, const String &b) noexcept
{
	if (a.size() != b.size()) return b.size() - a.size();
	else return memcmp(a.data(), b.data(), a.size());
};

const ir::String ir::operator+(const String &a, const String &b)
{
	String result;
	result.resize(a.size() + b.size());
	memcpy(result.data(), a.data(), a.size());
	memcpy(result.data() + a.size(), b.data(), b.size());
	return result;
};

#endif //#ifndef IR_STRING_IMPLEMENTATION