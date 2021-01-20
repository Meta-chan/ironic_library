/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_N2ST_DATABASE_IMPLEMENTATION
#define IR_N2ST_DATABASE_IMPLEMENTATION

#include <ir_resource/ir_memresource.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <share.h>
#endif

ir::N2STDatabase::MetaCell::MetaCell() noexcept
{
	offset = 0;
	size = 0;
	deleted = 0;
}

ir::ec ir::N2STDatabase::_read(void *buffer, unsigned int offset, unsigned int size) noexcept
{
	if (offset + size > _file.size) return ec::read_file;

	if (_file.hold)
	{
		memcpy(buffer, &_file.ram[offset], size);
	}
	else
	{
		if (offset != _file.pointer)
		{
			if (fseek(_file.file, offset, SEEK_SET) != 0) return ec::seek_file;
			_file.pointer = offset;
		}
		if (fread(buffer, size, 1, _file.file) == 0) return ec::read_file;
		_file.pointer += size;
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::_write(const void *buffer, unsigned int offset, unsigned int size) noexcept
{
	if (_file.hold)
	{
		if (offset + size > _file.size)
		{
			if (!_file.ram.resize(offset + size)) return ec::alloc;
			_file.size = offset + size;
		}
		if (size > 0)
		{
			memcpy(&_file.ram[offset], buffer, size);
			_file.changed = true;
		}
	}
	else
	{
		if (offset != _file.pointer)
		{
			if (fseek(_file.file, offset, SEEK_SET) != 0) return ec::seek_file;
			_file.pointer = offset;
		}
		if (fwrite(buffer, size, 1, _file.file) == 0) return ec::read_file;
		_file.pointer += size;
		if (offset + size > _file.size) _file.size = offset + size;
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::_readpointer(void **p, unsigned int offset, unsigned int size) noexcept
{
	if (offset + size > _file.size) return ec::read_file;

	if (_file.hold)
	{
		void *pointer = &_file.ram[offset];
		memcpy(p, &pointer, sizeof(void*));
	}
	else
	{
		//Actually openmap might change file pointer. It never causes a problem though
		fflush(_file.file);	//Openmap is native, database is not.
		//I DONT KNOW IF IT INCREASES SPEED! Test it!
		void *pointer = _mapping.map(_file.file, offset, size, Mapping::map_mode::read);
		if (pointer == nullptr) return ec::mapping;
		memcpy(p, &pointer, sizeof(void*));
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::_metaread(MetaCell *cell, unsigned int index) noexcept
{
	if (index >= _meta.size) return ec::read_file;

	if (_meta.hold)
	{
		*cell = _meta.ram[index];
	}
	else
	{
		if (index != _meta.pointer)
		{
			if (fseek(_meta.file, sizeof(MetaHeader) + index * sizeof(MetaCell), SEEK_SET) != 0) return ec::seek_file;
			_meta.pointer = index;
		}
		if (fread(cell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::read_file;
		_meta.pointer++;
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::_metawrite(MetaCell cell, unsigned int index) noexcept
{
	if (_meta.hold)
	{
		if (index >= _meta.size)
		{
			if (!_meta.ram.resize(index + 1)) return ec::alloc;
			_meta.size = index + 1;
		}
		_meta.ram[index] = cell;
		_meta.changed = true;
	}
	else
	{
		if (index != _meta.pointer)
		{
			if (fseek(_meta.file, sizeof(MetaHeader) + index * sizeof(MetaCell), SEEK_SET) != 0) return ec::seek_file;
			_meta.pointer = index;
		}
		//That memory is zero is guaranted by stdlib
		if (fwrite(&cell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::read_file;
		if (index >= _meta.size) _meta.size = index + 1;
		_meta.pointer++;
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::_check() noexcept
{
	//FILE
	_path[_path.size() - 2] = _beta ? 'c' : 'a';
	#ifdef _WIN32
		_file.file = _wfsopen(_path.data(), L"rb", _SH_DENYNO);
	#else
		_file.file = fopen(_path.data(), "rb");
	#endif

	if (_file.file == nullptr) return ec::open_file;
	FileHeader header, sample;
	if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::seek_file;
	if (fread(&header, sizeof(FileHeader), 1, _file.file) == 0			||
		memcmp(header.signature, sample.signature, 7) != 0				||
		header.version != sample.version) return ec::invalid_signature;
	
	if (fseek(_file.file, 0, SEEK_END) != 0) return ec::seek_file;
	_file.size = ftell(_file.file);
	_file.pointer = _file.size;

	//META
	_path[_path.size() - 2] = _beta ? 'd' : 'b';
	#ifdef _WIN32
		_meta.file = _wfsopen(_path.data(), L"rb", _SH_DENYNO);
	#else
		_meta.file = fopen(_path.data(), "rb");
	#endif

	if (_meta.file == nullptr) return ec::open_file;
	MetaHeader metaheader, metasample;
	if (fseek(_meta.file, 0, SEEK_SET) != 0) return ec::seek_file;
	if(fread(&metaheader, sizeof(MetaHeader), 1, _meta.file) == 0
	|| memcmp(metaheader.signature, metasample.signature, 7) != 0
	|| metaheader.version != metasample.version) return ec::invalid_signature;
	
	_file.used = metaheader.used;
	if (_file.used > _file.size) return ec::invalid_signature;

	if (fseek(_meta.file, 0, SEEK_END) != 0) return ec::seek_file;
	_meta.size = ftell(_meta.file) - sizeof(MetaHeader);
	if (_meta.size % sizeof(MetaCell) != 0) return ec::invalid_signature;
	_meta.size /= sizeof(MetaCell);
	_meta.pointer = _meta.size;

	_meta.count = metaheader.count;
	if (_meta.count > _meta.size) return ec::invalid_signature;

	return ec::ok;
}

ir::ec ir::N2STDatabase::_reopen_write(bool createnew) noexcept
{
	//FILE
	_path[_path.size() - 2] = _beta ? 'c' : 'a';
	if (_file.file != nullptr && createnew)
	{
		fclose(_file.file);
		_file.file = nullptr;
	}
	#ifdef _WIN32
		_file.file = _wfsopen(_path.data(), createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_file.file = fopen(_path.data(), createnew ? "w+b" : "r+b");
	#endif
	if (_file.file == nullptr) return ec::create_file;
	if (createnew)
	{
		FileHeader header;
		if (fwrite(&header, sizeof(FileHeader), 1, _file.file) == 0) return ec::write_file;
		_file.pointer = sizeof(FileHeader);
		_file.size = sizeof(FileHeader);
	}

	//META
	_path[_path.size() - 2] = _beta ? 'd' : 'b';
	if (_meta.file != nullptr && createnew)
	{
		fclose(_meta.file);
		_meta.file = nullptr;
	}
	#ifdef _WIN32
		_meta.file = _wfsopen(_path.data(), createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_meta.file = fopen(_path.data(), createnew ? "w+b" : "r+b");
	#endif
	if (_meta.file == nullptr) return ec::create_file;
	if (createnew)
	{
		MetaHeader header;
		if (fwrite(&header, sizeof(MetaHeader), 1, _meta.file) == 0) return ec::write_file;
		_meta.pointer = 0;
		_meta.size = 0;
	}

	_writeaccess = true;
	return ec::ok;
}

ir::ec ir::N2STDatabase::_init(const syschar *filepath, create_mode mode, bool opposite) noexcept
{
	#ifdef _WIN32
		size_t pathlen = wcslen(filepath);
	#else
		size_t pathlen = strlen(filepath);
	#endif

	if (!_path.resize(pathlen + 3)) return ec::alloc;
	memcpy(&_path[0], filepath, pathlen * sizeof(syschar));
	_path[pathlen] = '~';
	_path[pathlen + 1] = 'c';
	_path[pathlen + 2] = '\0';
	#ifdef _WIN32
		_beta = (_waccess(_path.data(), 0) == 0);
	#else
		_beta = (access(_path.data(), 0) == 0);
	#endif
	if (opposite) _beta = !_beta;

	if (mode == create_mode::neww)
	{
		ec code = _reopen_write(true);
		if (code != ec::ok) return code;
	}
	else
	{
		ec code = _check();
		if (code != ec::ok) return code;
		if (mode == create_mode::edit)
		{
			code = _reopen_write(false);
			if (code != ec::ok) return code;
		}
	}

	_ok = true;
	return ec::ok;
}

ir::N2STDatabase::N2STDatabase(const syschar *filepath, create_mode createmode, ec *code, bool) noexcept
{
	ec c = _init(filepath, createmode, true);
	if (code != nullptr) *code = c;
}

ir::N2STDatabase::N2STDatabase(const syschar *filepath, create_mode createmode, ec *code) noexcept
{
	ec c = _init(filepath, createmode, false);
	if (code != nullptr) *code = c;
}

bool ir::N2STDatabase::ok() const noexcept
{
	return _ok;
}

ir::ec ir::N2STDatabase::probe(unsigned int index) noexcept
{
	if (!_ok) return ec::object_not_inited;
	
	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	bool found = code == ec::ok && cell.offset != 0 && cell.deleted == 0;
	if (!found) return ec::key_not_exists;

	return ec::ok;
}

ir::ec ir::N2STDatabase::read(unsigned int index, ConstBlock *data) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (data == nullptr) return ec::null;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	
	bool found = code == ec::ok && cell.offset != 0 && cell.deleted == 0;
	if (!found) return ec::key_not_exists;

	//Read data
	void *readdata = nullptr;
	code = _readpointer(&readdata, cell.offset, cell.size);
	if (code != ec::ok) return code;
	
	*data = ConstBlock(readdata, cell.size);
	return ec::ok;
}

ir::ec ir::N2STDatabase::insert(unsigned int index, ConstBlock data, insert_mode mode) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	
	bool found = code == ec::ok && cell.offset != 0 && cell.deleted == 0;
	if (mode == insert_mode::existing && !found) return ec::key_not_exists;
	else if (mode == insert_mode::not_existing && found) return ec::key_already_exists;

	//If exists or deleted and size is sufficient
	unsigned int oldsize = cell.size;
	if (code == ec::ok && cell.offset != 0 && cell.size >= data.size())
	{
		//If something need to be changed
		if (cell.size != data.size() || cell.deleted > 0)
		{
			cell.size = data.size();
			cell.deleted = 0;
			code = _metawrite(cell, index);
			if (code != ec::ok) return code;
		}
	}
	else
	{
		cell.size = data.size();
		cell.deleted = 0;
		cell.offset = (_file.size + sizeof(unsigned int) - 1) & ~(sizeof(unsigned int) - 1);
		code = _metawrite(cell, index);
		if (code != ec::ok) return code;
	}
	code = _write(data.data(), cell.offset, (unsigned int)data.size());
	if (code != ec::ok) return code;

	if (found)
	{
		_file.used += _file.used + (unsigned int)data.size() - oldsize;
	}
	else
	{
		_file.used += (unsigned int)data.size();
		_meta.count++;
	}
	return ec::ok;
}

ir::ec ir::N2STDatabase::delet(unsigned int index, delete_mode mode) noexcept
{
	if (!_ok) return ec::object_not_inited; 
	if (!_writeaccess) return ec::write_file;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	bool found = code == ec::ok && cell.offset != 0 && cell.deleted == 0;

	if (!found)
	{
		if (mode == delete_mode::existing) return ec::key_not_exists;
		else return ec::ok;
	}
	else
	{
		cell.deleted = 1;
		code = _metawrite(cell, index);
		if (code != ec::ok) return code;
		if (found)
		{
			_meta.count--;
			_file.used -= cell.size;
		}
	}
	return ec::ok;
}

unsigned int ir::N2STDatabase::count() const noexcept
{
	if (!_ok) return 0;
	else return _meta.count;
}

unsigned int ir::N2STDatabase::get_table_size() const noexcept
{
	if (!_ok) return 0;
	else return _meta.size;
}

unsigned int ir::N2STDatabase::get_file_size() const noexcept
{
	if (!_ok) return 0;
	else return _file.size;
}

unsigned int ir::N2STDatabase::get_file_used_size() const noexcept
{
	if (!_ok) return 0;
	else return _file.used;
}

ir::ec ir::N2STDatabase::set_table_size(unsigned int newtablesize) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;
	else return ec::not_implemented;
}

ir::ec ir::N2STDatabase::set_file_size(unsigned int newfilesize) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;
	else return ec::not_implemented;
}

ir::ec ir::N2STDatabase::set_ram_mode(bool holdfile, bool holdmeta) noexcept
{
	if (!_ok) return ec::object_not_inited;

	//Read meta
	if (holdmeta && !_meta.hold)
	{
		if (!_meta.ram.resize(_meta.size)) return ec::alloc;
		if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::seek_file;;
		if (_meta.size != 0 && fread(&_meta.ram[0], sizeof(MetaCell), _meta.size, _meta.file) < _meta.size)
			return ec::read_file;
		_meta.pointer = _meta.size;
	}
	//Write meta
	else if (!holdmeta && _meta.hold)
	{
		if (_writeaccess && _meta.changed)
		{
			if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::seek_file;
			if (_meta.size != 0 && fwrite(&_meta.ram[0], sizeof(MetaCell), _meta.size, _meta.file) < _meta.size)
				return ec::write_file;
			_meta.pointer = _meta.size;
		}
		_meta.ram.clear();
	}
	_meta.hold = holdmeta;
	_meta.changed = false;

	//Read data
	if (holdfile && !_file.hold)
	{
		if (!_file.ram.resize(_file.size)) return ec::alloc;
		if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::seek_file;
		if (_file.size != 0 && fread(&_file.ram[0], 1, _file.size, _file.file) < _file.size)
			return ec::read_file;
		_file.pointer = _file.size;
	}
	//Write data
	else if (!holdfile && _file.hold)
	{
		if (_writeaccess && _file.changed)
		{
			if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::seek_file;;
			if (_file.size != 0 && fwrite(&_file.ram[0], 1, _file.size, _file.file) < _file.size)
				return ec::write_file;
			_file.pointer = _file.size;
		}
		_file.ram.clear();
	}
	_file.hold = holdfile;
	_file.changed = false;

	return ec::ok;
}

ir::ec ir::N2STDatabase::optimize() noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;
	{
		_path[_path.size() - 2] = '\0';
		ec code;
		N2STDatabase beta(_path.data(), create_mode::neww, &code, true);
		for (unsigned int i = 0; i < get_table_size(); i++)
		{
			ir::ConstBlock data;
			code = read(i, &data);
			if (code != ec::ok) continue;
			code = beta.insert(i, data);
			if (code != ec::ok) return code;
		}
		char buffer[sizeof(N2STDatabase)];
		memcpy(buffer, this, sizeof(N2STDatabase));
		memcpy(this, &beta, sizeof(N2STDatabase));
		memcpy(&beta, buffer, sizeof(N2STDatabase));
	}
	#ifdef _WIN32
		_path[_path.size() - 2] = _beta ? 'a' : 'c';
		_wunlink(_path.data());
		_path[_path.size() - 2] = _beta ? 'b' : 'd';
		_wunlink(_path.data());
	#else
		_path[path.size() - 2] = _beta ? 'a' : 'c';
		unlink(_path.data());
		_path[path.size() - 2] = _beta ? 'b' : 'd';
		unlink(_path.data());
	#endif
	return ec::ok;
}

ir::N2STDatabase::~N2STDatabase() noexcept
{
	_mapping.close();
	set_ram_mode(false, false);
	if (_file.file != nullptr) fclose(_file.file);
	if (_meta.file != nullptr)
	{
		if (_writeaccess)
		{
			MetaHeader header;
			header.count = _meta.count;
			header.used = _file.used;
			fseek(_meta.file, 0, SEEK_SET);
			fwrite(&header, sizeof(MetaHeader), 1, _meta.file);
		}
		fclose(_meta.file);
	}
}

#endif	//#ifndef IR_N2ST_DATABASE_IMPLEMENTATION