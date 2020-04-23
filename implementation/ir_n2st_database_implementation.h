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
#include "ir_resource.h"
#include "ir_reserve.h"

#define IR_T_DATABASE_TYPE N2STDatabase
#include "implementation/ir_t_database_implementation.h"

ir::ec ir::N2STDatabase::_metawrite(unsigned int keyoffset, unsigned int index)
{
	if (_holdmeta)
	{
		if (index >= _tablesize)
		{
			if (_rammetafile == nullptr) _rammetafile = (unsigned int*)malloc((index + 1) * sizeof(unsigned int));
			else _rammetafile = (unsigned int*)realloc(_rammetafile, (index + 1) * sizeof(unsigned int));
			if (_rammetafile == nullptr) return ec::ec_alloc;
			memset(_rammetafile + _tablesize, 0, (index - _tablesize) * sizeof(unsigned int));
			_tablesize = (index + 1);
		}
		_rammetafile[index] = keyoffset;
		_metachanged = true;
	}
	else
	{
		if (index != _metapointer)
		{
			if (fseek(_metafile, sizeof(MetaFileHeader) + index * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
			_metapointer = index;
		}
		//Need to write zeros!
		if (fwrite(&keyoffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_read_file;
		if (index >= _tablesize) _tablesize = index + 1;
		_metapointer++;
	}
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_checkfile(const syschar *filepath)
{
	#ifdef _WIN32
		_file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		_file = fopen(filepath, "rb");
	#endif

	if (_file == nullptr) return ec::ec_open_file;
	FileHeader header;
	if (fseek(_file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&header, sizeof(FileHeader), 1, _file) == 0				||
		memcmp(header.signature, "IN2STDF", 7) != 0						||
		header.version != 0) return ec::ec_invalid_signature;
	
	if (fseek(_file, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_filesize = ftell(_file);

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_checkmetafile(const syschar *filepath)
{
	#ifdef _WIN32
		_metafile = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		_metafile = fopen(filepath, "rb");
	#endif

	if (_metafile == nullptr) return ec::ec_open_file;
	MetaFileHeader metaheader;
	if (fseek(_metafile, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&metaheader, sizeof(MetaFileHeader), 1, _metafile) == 0	||
		memcmp(metaheader.signature, "IN2STDM", 7) != 0					||
		metaheader.version != 0) return ec::ec_invalid_signature;
	
	_count = metaheader.count;
	if (fseek(_metafile, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_tablesize = ftell(_metafile) - sizeof(MetaFileHeader);
	if (_tablesize % sizeof(unsigned int) != 0) return ec::ec_invalid_signature;
	_tablesize /= sizeof(unsigned int);

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::_openwrite(const syschar *filepath, const syschar *metafilepath, bool createnew)
{
	//File
	if (_file != nullptr && createnew)
	{
		fclose(_file);
		_file = nullptr;
	}
	#ifdef _WIN32
		_file = _wfsopen(filepath, createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_file = fopen(filepath, createnew ? "w+b" : "r+b");
	#endif
	if (_file == nullptr) return ec::ec_create_file;
	if (createnew)
	{
		FileHeader header;
		if (fwrite(&header, sizeof(FileHeader), 1, _file) == 0) return ec::ec_write_file;
		_filepointer = sizeof(FileHeader);
		_filesize = sizeof(FileHeader);
	}

	//Metafile
	if (_metafile != nullptr && createnew)
	{
		fclose(_metafile);
		_metafile = nullptr;
	}
	#ifdef _WIN32
		_metafile = _wfsopen(metafilepath, createnew ? L"w+b" : L"r+b", _SH_DENYNO);
	#else
		_metafile = fopen(metafilepath, createnew ? "w+b" : "r+b");
	#endif
	if (_metafile == nullptr) return ec::ec_create_file;
	if (createnew)
	{
		MetaFileHeader header;
		if (fwrite(&header, sizeof(MetaFileHeader), 1, _metafile) == 0) return ec::ec_write_file;
		_metapointer = 0;
		_tablesize = 0;
	}

	_writeaccess = true;
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::read(unsigned int index, ConstBlock *data)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (data == nullptr) return ec::ec_null;

	//Read data offset
	unsigned int dataoffset = 0;
	ec code = _metaread(&dataoffset, index);
	if (code != ec::ec_ok) return code;
	if (dataoffset == 0) return ec::ec_key_not_exists;
	
	//Read data size
	unsigned int datasize = 0;
	unsigned int headersize = 0;
	code = _readsize(dataoffset, &datasize, &headersize);
	if (code != ec::ec_ok) return code;

	//Read data
	void *readdata = nullptr;
	code = _readpointer(&readdata, dataoffset + headersize, datasize);
	if (code != ec::ec_ok) return code;
	
	data->data = readdata;
	data->size = datasize;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::insert(unsigned int index, ConstBlock data, insertmode mode)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (!_writeaccess) return ec::ec_write_file;

	if (mode != insertmode::insert_always)
	{
		//Read old offset
		unsigned int oldoffset = 0;
		ec code = _metaread(&oldoffset, index);
		if (mode == insertmode::insert_existing)
		{
			if (code != ec::ec_ok || oldoffset == 0) return ec::ec_key_not_exists;
		}
		else
		{
			if (code == ec::ec_ok && oldoffset != 0) return ec::ec_key_already_exists;
		}
	}

	//Write data
	unsigned int dataoffset = 0;
	ec code = _writeblock(data, &dataoffset);
	if (code != ec::ec_ok) return code;

	//Write offset
	code = _metawrite(dataoffset, index);
	if (code != ec::ec_ok) return code;

	_count++;
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::delet(unsigned int index, deletemode mode)
{
	if (!_ok) return ec::ec_object_not_ok; 
	if (!_writeaccess) return ec::ec_write_file;

	if (mode == deletemode::delete_existing)
	{
		//Read old offset
		unsigned int oldoffset = 0;
		ec code = _metaread(&oldoffset, index);
		if (code != ec::ec_ok || oldoffset == 0) return ec::ec_key_not_exists;
	}

	//Write nulloffset to metafile
	ec code = _metawrite(0, index);
	if (code != ec::ec_ok) return code;

	_count--;
	return ec::ec_ok;
};

ir::N2STDatabase::~N2STDatabase()
{
	closemap(&_mapcache);
	setrammode(false, false);
	if (_file != nullptr) fclose(_file);
	if (_metafile != nullptr)
	{
		if (_writeaccess)
		{
			fseek(_metafile, offsetof(MetaFileHeader, count), SEEK_SET);
			fwrite(&_count, sizeof(unsigned int), 1, _metafile);
		}
		fclose(_metafile);
	}
};

#endif	//#ifndef IR_N2ST_DATABASE_IMPLEMENTATION