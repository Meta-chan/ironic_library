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
#include <assert.h>

ir::File::File() noexcept
{
}

ir::File::File(const schar *filename, const schar *mode) noexcept
{
	open(filename, mode);
}

ir::File::File(const File &file) noexcept
{
	_header = file._header;
	if (_header != nullptr)
	{
		_header->refcount++;
	}
}

const ir::File &ir::File::assign(const File &file) noexcept
{
	if (_header != file._header)
	{
		close();
		_header = file._header;
		if (_header != nullptr)
		{
			_header->refcount++;
		}
	}
	return *this;
}

const ir::File &ir::File::operator=(const File &file) noexcept
{
	return assign(file);
}

bool ir::File::open(const schar *filename, const schar *mode) noexcept
{
	close();
	_header = (Header*)malloc(sizeof(Header));
	if (_header == nullptr) return false;
	_header->file = nullptr;
	_header->refcount = 1;
	_header->position = (uint64)-1;

	#ifdef _WIN32
		_wfopen_s(&_header->file, filename, mode);
	#else
		FILE *_header->file = fopen(filename, mode);
	#endif

	if (_header->file == nullptr) return false;
	_header->position = ftell(_header->file);
	return true;
}

void ir::File::close() noexcept
{
	if (_header != nullptr)
	{
		if (_header->refcount > 1) _header->refcount--;
		else
		{
			if (_header->file != nullptr) fclose(_header->file);
			free(_header);
		}
		_header = nullptr;
	}
}

bool ir::File::seek(uint64 position, int mode) noexcept
{
	assert(ok());
	if (fseek(_header->file, (long)position, mode) == 0) return true;
	_header->position = (uint64)-1;
	return false;
}

ir::uint64 ir::File::tell() const noexcept
{
	assert(ok());
	if (_header->position != (uint64)-1) return _header->position;
	_header->position = ftell(_header->file);
	return _header->position;
}

ir::uint64 ir::File::size() noexcept
{
	uint64 position = tell();
	assert(seek(0, SEEK_END));
	uint64 size = tell();
	assert(seek(position, SEEK_SET));
	return size;
}

bool ir::File::ok() const noexcept
{
	return _header != nullptr && _header->file != nullptr;
}

FILE *ir::File::file() noexcept
{
	if (_header == nullptr) return nullptr;
	else return _header->file;
}

size_t ir::File::read(void *data, size_t size) noexcept
{
	assert(ok());
	size_t read = fread(data, 1, size, _header->file);
	_header->position += read;
	return read;
}

size_t ir::File::write(const void *data, size_t size) noexcept
{
	assert(ok());
	size_t wrote = fwrite(data, 1, size, _header->file);
	_header->position += wrote;
	return wrote;
}

ir::File::~File() noexcept
{
	close();
}