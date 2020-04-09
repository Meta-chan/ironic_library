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

class MemFreer{ public: static void free(void *mem) { ::free(mem); } };
typedef ir::Resource<ir::syschar*, MemFreer, nullptr> StringResource;

ir::ec ir::N2STDatabase::_openfile(const syschar *filepath, bool meta, accessmode amode)
{
	FILE *file = nullptr;

	#ifdef _WIN32
		switch (amode)
		{
		case accessmode::access_new:
			file = _wfsopen(filepath, L"w+b", _SH_DENYNO); break;
		case accessmode::access_readwrite:
			file = _wfsopen(filepath, L"a+b", _SH_DENYNO); break;
		case accessmode::access_read:
			file = _wfsopen(filepath, L"rb", _SH_DENYNO); break;
		}
	#else
		switch (amode)
		{
		case accessmode::access_new:
			file = fpen(filepath, "w+b"); break;
		case accessmode::access_readwrite:
			file = fopen(filepath, "a+b"); break;
		case accessmode::access_read:
			file = fopen(filepath, "rb"); break;
		}
	#endif
	
	if (file == nullptr) return ec::ec_open_file;
	else if (meta) _metafile = file;
	else _file = file;

	if (amode == accessmode::access_new)
	{
		if (fseek(file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
		if (meta)
		{
			MetaFileHeader metaheader;
			if (fwrite(&metaheader, sizeof(MetaFileHeader), 1, _metafile) == 0) return ec::ec_write_file;
		}
		else
		{
			FileHeader header;
			if (fwrite(&header, sizeof(FileHeader), 1, _file) == 0) return ec::ec_write_file;
		}
	}
	else
	{
		if (fseek(file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
		if (meta)
		{
			MetaFileHeader metaheader;
			if (fread(&metaheader, sizeof(MetaFileHeader), 1, _metafile) == 0	||
				memcmp(metaheader.signature, "IN2STDM", 7) != 0					||
				metaheader.version != 0) return ec::ec_invalid_signature;
			_count = metaheader.count;
		}
		else
		{
			FileHeader header;
			if (fread(&header, sizeof(FileHeader), 1, _file) == 0			||
				memcmp(header.signature, "IN2STDF", 7) != 0					||
				header.version != 0) return ec::ec_invalid_signature;
		}
		
	}
	return ec::ec_ok;
}

ir::ec ir::N2STDatabase::_init(const syschar *filepath, accessmode amode)
{
	unsigned int pathlen = wcslen(filepath);
	StringResource metafilepath = (syschar*)malloc((pathlen + 2) * sizeof(syschar));
	if (metafilepath.it == nullptr) return ec::ec_alloc;
	memcpy(metafilepath.it, filepath, pathlen * sizeof(syschar));
	metafilepath.it[pathlen] = '~';
	metafilepath.it[pathlen + 1] = '\0';
	
	ec code = _openfile(filepath, false, amode);
	if (code != ec::ec_ok) return code;

	code = _openfile(metafilepath.it, true, amode);
	if (code != ec::ec_ok) return code;

	_ok = true;
	return ec::ec_ok;
};

ir::N2STDatabase::N2STDatabase(const syschar *filepath, accessmode mode, rammode rmode, ec *code)
{
	ec c = _init(filepath, mode);
	if (code != nullptr) *code = c;
};

unsigned int ir::N2STDatabase::count()
{
	if (!_ok) return 0;
	else return _count;
};

unsigned int ir::N2STDatabase::tablesize()
{
	if (!_ok) return 0;
	if (fseek(_metafile, 0, SEEK_END) != 0) return 0;
	return ((ftell(_metafile) - sizeof(MetaFileHeader)) / sizeof(unsigned int));
};

ir::ec ir::N2STDatabase::setrammode(rammode rmode)
{
	//Sorry )
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::read(unsigned int number, ConstBlock *data)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (data == nullptr) return ec::ec_null;

	//reading file offset in metafile
	if (fseek(_metafile, sizeof(MetaFileHeader) + number * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_key_not_exists;
	unsigned int offset = 0;
	if (fread(&offset, sizeof(unsigned int), 1, _metafile) == 0 || offset == 0) return ec::ec_key_not_exists;

	//reading data in file
	if (fseek(_file, offset, SEEK_SET) != 0) return ec::ec_key_not_exists;
	unsigned int size = 0;
	if (fread(&size, 1, 1, _file) == 0) return ec::ec_key_not_exists;
	if (size > 251)
	{
		if (fread(&size, size - 251, 1, _file) == 0) return ec::ec_key_not_exists; //size > 5151, no heck
	}
	if (!reserve(&_buffer.data, &_buffer.size, size)) return ec::ec_alloc;
	if (size != 0 && fread(_buffer.data, size, 1, _file) == 0) return ec::ec_key_not_exists;
	
	data->data = _buffer.data;
	data->size = size;

	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::insert(unsigned int number, ConstBlock data, insertmode mode)
{
	if (mode != insertmode::insert_always)
	{
		//reading offet from metafile
		unsigned int oldoffset = 0;
		if (fseek(_metafile, sizeof(MetaFileHeader) + number * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
		if (mode == insertmode::insert_existing)
		{
			if (fread(&oldoffset, sizeof(unsigned int), 1, _metafile) == 0 || oldoffset == 0) return ec::ec_key_not_exists;
		}
		else
		{
			if (fread(&oldoffset, sizeof(unsigned int), 1, _metafile) != 0 && oldoffset != 0) return ec::ec_key_already_exists;
		}
	}

	//writing data to file
	if (fseek(_file, 0, SEEK_END) != 0) return ec::ec_seek_file;
	unsigned int offset = ftell(_file);
	if (data.size > 251)
	{
		unsigned char nbytes;
		if (data.size > 256 * 256 * 256) nbytes = 4;
		else if (data.size > 256 * 256) nbytes = 3;
		else if (data.size > 256) nbytes = 2;
		else nbytes = 1;

		unsigned int nbytestowrite = nbytes + 251;
		if (fwrite(&nbytestowrite, 1, 1, _file) == 0) return ec::ec_write_file;
		if (fwrite(&data.size, nbytes, 1, _file) == 0) return ec::ec_write_file; //nbytes > 1, no check
	}
	else
	{
		if (fwrite(&data.size, 1, 1, _file) == 0) return ec::ec_write_file;
	}
	if (data.size != 0 && fwrite(data.data, data.size, 1, _file) == 0) return ec::ec_write_file;

	//writing offet to metafile
	if (fseek(_metafile, sizeof(MetaFileHeader) + number * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
	if (fwrite(&offset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_write_file;

	_count++;
	return ec::ec_ok;
};

ir::ec ir::N2STDatabase::delet(unsigned int number, deletemode mode)
{
	if (mode == deletemode::delete_existing)
	{
		//reading offset from metafile
		unsigned int oldoffset = 0;
		if (fseek(_metafile, sizeof(MetaFileHeader) + number * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
		if (mode == deletemode::delete_existing)
		{
			if (fread(&oldoffset, sizeof(unsigned int), 1, _metafile) == 0 || oldoffset == 0) return ec::ec_key_not_exists;
		}
	}

	//write nulloffset to metafile
	if (fseek(_metafile, sizeof(MetaFileHeader) + number * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
	
	unsigned int nulloffset = 0;
	if (fwrite(&nulloffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_write_file;

	_count--;
	return ec::ec_ok;
};

ir::N2STDatabase::~N2STDatabase()
{
	if (_file != nullptr) fclose(_file);
	if (_metafile != nullptr)
	{
		fseek(_metafile, offsetof(MetaFileHeader, count), SEEK_SET);
		fwrite(_metafile, sizeof(unsigned int), 1, _metafile);
		fclose(_metafile);
	}
};

#endif	//#ifndef IR_N2ST_DATABASE_IMPLEMENTATION