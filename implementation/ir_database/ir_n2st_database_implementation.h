/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_N2ST_DATABASE_IMPLEMENTATION
#define IR_N2ST_DATABASE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <share.h>
#endif
#include <ir_resource/ir_memresource.h>
#include <ir_reserve.h>

ir::N2STDatabase::MetaCell::MetaCell()
{
	offset = 0;
	size = 0;
	deleted = 0;
};

ir::ec ir::N2STDatabase::_read(void *buffer, unsigned int offset, unsigned int size)
{
	if (offset + size > _file.size) return ec::ec_read_file;

	if (_file.hold)
	{
		memcpy(buffer, (char*)_file.ram + offset, size);
	}
	else
	{
		if (offset != _file.pointer)
		{
			if (fseek(_file.file, offset, SEEK_SET) != 0) return ec::ec_seek_file;
			_file.pointer = offset;
		}
		if (fread(buffer, size, 1, _file.file) == 0) return ec::ec_read_file;
		_file.pointer += size;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_write(const void *buffer, unsigned int offset, unsigned int size)
{
	if (_file.hold)
	{
		if (offset + size > _file.size)
		{
			if (!reserve(&_file.ram, &_file.size, offset + size)) return ec::ec_alloc;
		};
		memcpy((char*)_file.ram + offset, buffer, size);
		if (size > 0) _file.changed = true;
	}
	else
	{
		if (offset != _file.pointer)
		{
			if (fseek(_file.file, offset, SEEK_SET) != 0) return ec::ec_seek_file;
			_file.pointer = offset;
		}
		if (fwrite(buffer, size, 1, _file.file) == 0) return ec::ec_read_file;
		_file.pointer += size;
		if (offset + size > _file.size) _file.size = offset + size;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_readpointer(void **p, unsigned int offset, unsigned int size)
{
	if (offset + size > _file.size) return ec::ec_read_file;

	if (_file.hold)
	{
		void *pointer = (char*)_file.ram + offset;
		memcpy(p, &pointer, sizeof(void*));
	}
	else
	{
		//Actually openmap might change file pointer. It never causes a problem though
		fflush(_file.file);	//Openmap is native, database is not.
		//I DONT KNOW IF IT INCREASES SPEED! Test it!
		void *pointer = openmap(&_mapcache, _file.file, offset, size, openmapmode::openmap_read);
		if (pointer == nullptr) return ec::ec_openmap;
		memcpy(p, &pointer, sizeof(void*));
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_metaread(MetaCell *cell, unsigned int index)
{
	if (index >= _meta.size) return ec::ec_read_file;

	if (_meta.hold)
	{
		*cell = _meta.ram[index];
	}
	else
	{
		if (index != _meta.pointer)
		{
			if (fseek(_meta.file, sizeof(MetaHeader) + index * sizeof(MetaCell), SEEK_SET) != 0) return ec::ec_seek_file;
			_meta.pointer = index;
		}
		if (fread(cell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::ec_read_file;
		_meta.pointer++;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_metawrite(MetaCell cell, unsigned int index)
{
	if (_meta.hold)
	{
		if (index >= _meta.size)
		{
			if (_meta.ram == nullptr) _meta.ram = (MetaCell*)malloc((index + 1) * sizeof(MetaCell));
			else _meta.ram = (MetaCell*)realloc(_meta.ram, (index + 1) * sizeof(MetaCell));
			if (_meta.ram == nullptr) return ec::ec_alloc;
			memset(_meta.ram + _meta.size, 0, (index - _meta.size) * sizeof(MetaCell));
			_meta.size = (index + 1);
		}
		_meta.ram[index] = cell;
		_meta.changed = true;
	}
	else
	{
		if (index != _meta.pointer)
		{
			if (fseek(_meta.file, sizeof(MetaHeader) + index * sizeof(MetaCell), SEEK_SET) != 0) return ec::ec_seek_file;
			_meta.pointer = index;
		}
		//That memory is zero is guaranted by stdlib
		if (fwrite(&cell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::ec_read_file;
		if (index >= _meta.size) _meta.size = index + 1;
		_meta.pointer++;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_check(const syschar *filepath, const syschar *metapath)
{
	//FILE
	#ifdef _WIN32
		_file.file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		_file.file = fopen(filepath, "rb");
	#endif

	if (_file.file == nullptr) return ec::ec_open_file;
	FileHeader header;
	if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&header, sizeof(FileHeader), 1, _file.file) == 0			||
		memcmp(header.signature, "IN2STDF", 7) != 0						||
		header.version != 1) return ec::ec_invalid_signature;
	
	if (fseek(_file.file, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_file.size = ftell(_file.file);
	_file.pointer = _file.size;

	//META
	#ifdef _WIN32
		_meta.file = _wfsopen(metapath, L"rb", _SH_DENYNO);
	#else
		_meta.file = fopen(metapath, "rb");
	#endif

	if (_meta.file == nullptr) return ec::ec_open_file;
	MetaHeader metaheader;
	if (fseek(_meta.file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&metaheader, sizeof(MetaHeader), 1, _meta.file) == 0		||
		memcmp(metaheader.signature, "IN2STDM", 7) != 0					||
		metaheader.version != 1) return ec::ec_invalid_signature;
	
	_file.used = metaheader.used;
	if (_file.used > _file.size) return ec::ec_invalid_signature;

	if (fseek(_meta.file, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_meta.size = ftell(_meta.file) - sizeof(MetaHeader);
	if (_meta.size % sizeof(MetaCell) != 0) return ec::ec_invalid_signature;
	_meta.size /= sizeof(MetaCell);
	_meta.pointer = _meta.size;

	_meta.count = metaheader.count;
	if (_meta.count > _meta.size) return ec::ec_invalid_signature;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_openwrite(const syschar *filepath, const syschar *metapath, bool createnew)
{
	//FILE
	if (_file.file != nullptr && createnew)
	{
		fclose(_file.file);
		_file.file = nullptr;
	}
	#ifdef _WIN32
		_file.file = _wfsopen(filepath, createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_file.file = fopen(filepath, createnew ? "w+b" : "r+b");
	#endif
	if (_file.file == nullptr) return ec::ec_create_file;
	if (createnew)
	{
		FileHeader header;
		if (fwrite(&header, sizeof(FileHeader), 1, _file.file) == 0) return ec::ec_write_file;
		_file.pointer = sizeof(FileHeader);
		_file.size = sizeof(FileHeader);
	}

	//META
	if (_meta.file != nullptr && createnew)
	{
		fclose(_meta.file);
		_meta.file = nullptr;
	}
	#ifdef _WIN32
		_meta.file = _wfsopen(metapath, createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_meta.file = fopen(metapath, createnew ? "w+b" : "r+b");
	#endif
	if (_meta.file == nullptr) return ec::ec_create_file;
	if (createnew)
	{
		MetaHeader header;
		if (fwrite(&header, sizeof(MetaHeader), 1, _meta.file) == 0) return ec::ec_write_file;
		_meta.pointer = 0;
		_meta.size = 0;
	}

	_writeaccess = true;
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_init(const syschar *filepath, createmode cmode)
{
	#ifdef _WIN32
		unsigned int pathlen = wcslen(filepath);
	#else
		unsigned int pathlen = strlen(filepath);
	#endif
	
	MemResource<syschar> metafilepath = (syschar*)malloc((pathlen + 2) * sizeof(syschar));
	if (metafilepath == nullptr) return ec::ec_alloc;
	memcpy(metafilepath, filepath, pathlen * sizeof(syschar));
	metafilepath[pathlen] = '~';
	metafilepath[pathlen + 1] = '\0';
	
	ec filestatus = _check(filepath, metafilepath);
	
	//Hehehehe. I am sorry(
	ec code = ec::ec_ok;
	switch (cmode)
	{
	case createmode::create_readonly:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus != ec::ec_ok) return ec::ec_invalid_signature;
		else break;
	case createmode::create_new_never:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus == ec::ec_invalid_signature) return ec::ec_invalid_signature;
		else code = _openwrite(filepath, metafilepath, false);
		break;
	case createmode::create_new_if_no:
		if (filestatus == ec::ec_open_file) code = _openwrite(filepath, metafilepath, true);
		else if (filestatus == ec::ec_invalid_signature) return ec::ec_invalid_signature;
		else code = _openwrite(filepath, metafilepath, false);
		break;
	case createmode::create_new_if_corrupted:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus == ec::ec_invalid_signature) code = _openwrite(filepath, metafilepath, true);
		else code = _openwrite(filepath, metafilepath, false);
		break;
	case createmode::create_new_if_no_or_corrupted:
		if (filestatus == ec::ec_open_file) code = _openwrite(filepath, metafilepath, true);
		if (filestatus == ec::ec_invalid_signature) code = _openwrite(filepath, metafilepath, true);
		code = _openwrite(filepath, metafilepath, false);
		break;
	case createmode::create_new_always:
		code = _openwrite(filepath, metafilepath, true);
		break;
	}

	if (code != ec::ec_ok) return code;
	_ok = true;
	return ec::ec_ok;
};

ir::N2STDatabase::N2STDatabase(const syschar *filepath, createmode createmode, ec *code)
{
	ec c = _init(filepath, createmode);
	if (code != nullptr) *code = c;
};

ir::ec ir::N2STDatabase::probe(unsigned int index)
{
	if (!_ok) return ec::ec_object_not_ok;
	
	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	bool found = code == ec::ec_ok && cell.offset != 0 && cell.deleted == 0;
	if (!found) return ec::ec_key_not_exists;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::read(unsigned int index, ConstBlock *data)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (data == nullptr) return ec::ec_null;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	
	bool found = code == ec::ec_ok && cell.offset != 0 && cell.deleted == 0;
	if (!found) return ec::ec_key_not_exists;

	//Read data
	void *readdata = nullptr;
	code = _readpointer(&readdata, cell.offset, cell.size);
	if (code != ec::ec_ok) return code;
	
	data->data = readdata;
	data->size = cell.size;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::insert(unsigned int index, ConstBlock data, insertmode mode)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (!_writeaccess) return ec::ec_write_file;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	
	bool found = code == ec::ec_ok && cell.offset != 0 && cell.deleted == 0;
	if (mode == insertmode::insert_existing && !found) return ec::ec_key_not_exists;
	else if (mode == insertmode::insert_not_existing && found) return ec::ec_key_already_exists;

	//If exists or deleted and size is sufficient
	unsigned int oldsize = cell.size;
	if (code == ec::ec_ok && cell.offset != 0 && cell.size >= data.size)
	{
		//If something need to be changed
		if (cell.size != data.size || cell.deleted > 0)
		{
			cell.size = data.size;
			cell.deleted = 0;
			code = _metawrite(cell, index);
			if (code != ec::ec_ok) return code;
		}
	}
	else
	{
		cell.size = data.size;
		cell.deleted = 0;
		cell.offset = (_file.size + sizeof(unsigned int) - 1) & ~(sizeof(unsigned int) - 1);
		code = _metawrite(cell, index);
		if (code != ec::ec_ok) return code;
	}
	code = _write(data.data, cell.offset, data.size);
	if (code != ec::ec_ok) return code;

	if (found)
	{
		_file.used += _file.used + data.size - oldsize;
	}
	else
	{
		_file.used += data.size;
		_meta.count++;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::delet(unsigned int index, deletemode mode)
{
	if (!_ok) return ec::ec_object_not_ok; 
	if (!_writeaccess) return ec::ec_write_file;

	//Read offset & size
	MetaCell cell;
	ec code = _metaread(&cell, index);
	bool found = code == ec::ec_ok && cell.offset != 0 && cell.deleted == 0;

	if (!found)
	{
		if (mode == deletemode::delete_existing) return ec::ec_key_not_exists;
		else return ec::ec_ok;
	}
	else
	{
		cell.deleted = 1;
		code = _metawrite(cell, index);
		if (code != ec::ec_ok) return code;
		if (found)
		{
			_meta.count--;
			_file.used -= cell.size;
		}
	}
	return ec::ec_ok;
};

unsigned int ir::N2STDatabase::count()
{
	if (!_ok) return 0;
	else return _meta.count;
};

unsigned int ir::N2STDatabase::get_table_size()
{
	if (!_ok) return 0;
	else return _meta.size;
};

unsigned int ir::N2STDatabase::get_file_size()
{
	if (!_ok) return 0;
	else return _file.size;
};

unsigned int ir::N2STDatabase::get_file_used_size()
{
	if (!_ok) return 0;
	else return _file.used;
};

ir::ec ir::N2STDatabase::set_table_size(unsigned int newtablesize)
{
	if (!_ok) return ec::ec_object_not_ok;
	else return ec::ec_not_implemented;
};

ir::ec ir::N2STDatabase::set_file_size(unsigned int newfilesize)
{
	if (!_ok) return ec::ec_object_not_ok;
	else return ec::ec_not_implemented;
};

ir::ec ir::N2STDatabase::set_ram_mode(bool holdfile, bool holdmeta)
{
	if (!_ok) return ec::ec_object_not_ok;

	//Read meta
	if (holdmeta && !_meta.hold)
	{
		_meta.ram = (MetaCell*)malloc(_meta.size * sizeof(MetaCell));
		if (_meta.ram == nullptr) return ec::ec_alloc;
		if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fread(_meta.ram, sizeof(MetaCell), _meta.size, _meta.file) < _meta.size) return ec::ec_read_file;
		_meta.pointer = _meta.size;
	}
	//Write meta
	else if (!holdmeta && _meta.hold)
	{
		if (_writeaccess && _meta.changed)
		{
			if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::ec_seek_file;
			if (fwrite(_meta.ram, sizeof(MetaCell), _meta.size, _meta.file) < _meta.size) return ec::ec_write_file;
			_meta.pointer = _meta.size;
		}
		free(_meta.ram);
		_meta.ram = nullptr;
	}
	_meta.hold = holdmeta;
	_meta.changed = false;

	//Read data
	if (holdfile && !_file.hold)
	{
		_file.ram = malloc(_file.size);
		if (_file.ram == nullptr) return ec::ec_alloc;
		if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fread(_file.ram, 1, _file.size, _file.file) < _file.size) return ec::ec_read_file;;
		_file.pointer = _file.size;
	}
	//Write data
	else if (!holdfile && _file.hold)
	{
		if (_writeaccess && _file.changed)
		{
			if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::ec_seek_file;;
			if (fwrite(_file.ram, 1, _file.size, _file.file) < _file.size) return ec::ec_write_file;
			_file.pointer = _file.size;
		}
		free(_file.ram);
		_file.ram = nullptr;
	}
	_file.hold = holdfile;
	_file.changed = false;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::optimize()
{
	if (!_ok) return ec::ec_object_not_ok;
	else return ec::ec_not_implemented;
};

ir::N2STDatabase::~N2STDatabase()
{
	closemap(&_mapcache);
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
			fwrite(&_meta.count, sizeof(MetaHeader), 1, _meta.file);
		}
		fclose(_meta.file);
	}
};

#endif	//#ifndef IR_N2ST_DATABASE_IMPLEMENTATION