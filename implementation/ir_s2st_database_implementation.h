/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_S2ST_DATABASE_IMPLEMENTATION
#define IR_S2ST_DATABASE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <share.h>
#endif
#include "ir_resource.h"
#include "ir_reserve.h"
#include "ir_md5.h"

#define IR_T_DATABASE_TYPE S2STDatabase
#include "implementation/ir_t_database_implementation.h"

ir::ec ir::IR_T_DATABASE_TYPE::_metawrite(unsigned int keyoffset, unsigned int index)
{
	if (index >= _tablesize) return ec::ec_read_file;

	if (_holdmeta)
	{
		_rammetafile[index] = keyoffset;
	}
	else
	{
		if (index != _metapointer)
		{
			if (fseek(_metafile, sizeof(MetaFileHeader) + index * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
			_metapointer = index;
		}
		if (fwrite(&keyoffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_read_file;
		_metapointer++;
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_checkfile(const syschar *filepath)
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
		memcmp(header.signature, "IS2STDF", 7) != 0						||
		header.version != 0) return ec::ec_invalid_signature;
	
	if (fseek(_file, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_filesize = ftell(_file);

	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_checkmetafile(const syschar *filepath)
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
		memcmp(metaheader.signature, "IS2STDM", 7) != 0					||
		metaheader.version != 0) return ec::ec_invalid_signature;
	
	_count = metaheader.count;
	_delcount = metaheader.delcount;
	if (fseek(_metafile, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_tablesize = ftell(_metafile) - sizeof(MetaFileHeader);
	if (_tablesize < sizeof(unsigned int) || (_tablesize & (_tablesize - 1) == 0)) return ec::ec_invalid_signature;
	_tablesize /= sizeof(unsigned int);

	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_openwrite(const syschar *filepath, const syschar *metafilepath, bool createnew)
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
		unsigned int nulloffset = 0;
		if (fwrite(&nulloffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_write_file;
		_metapointer = 1;
		_tablesize = 1;
	}

	_writeaccess = true;
	return ec::ec_ok;
};

//index gets an offset in metafile where to write offset
//dataoffset is data offset read from index or 0/1 if not exists/deleted
ir::ec ir::S2STDatabase::_find(ConstBlock key, unsigned int *index, unsigned int *dataoffset)
{
	//Calculate hash
	unsigned int hash[4];
	ir::md5(key.data, key.size, hash);
	*index = hash[0] & (_tablesize - 1);

	//Search for free space in metafile
	*dataoffset = 0;
	while (true)
	{
		//Reading file offset in metafile. If eof go to begin
		unsigned int keyoffset = 0;
		ec code = _metaread(&keyoffset, *index);
		if (code != ec::ec_ok) return code;

		//Check if not exists, deleted or exists
		if (keyoffset == 0)
		{
			*dataoffset = 0;
			break;
		}
		else if (keyoffset == 1)
		{
			*dataoffset = 1;
		}
		else
		{
			//Check if key matches
			//Read key size
			unsigned int keysize = 0;
			unsigned int headersize = 0;
			ec code = _readsize(keyoffset, &keysize, &headersize);
			if (code != ec::ec_ok) return code;
	
			//Check key size
			if (key.size == keysize)
			{
				void *readkey = nullptr;
				code = _readpointer(&readkey, keyoffset + headersize, keysize);
				if (code != ec::ec_ok) return code;
				if (memcmp(key.data, _buffer.data, keysize) == 0)
				{
					*dataoffset = keyoffset + headersize + keysize;
					break;
				};
			}
		}

		(*index)++;
		if (*index == _tablesize) *index = 0;
	}
	return ec::ec_ok;
}

ir::ec ir::S2STDatabase::_rehash(unsigned int newtablesize)
{
	UintResource table = (unsigned int*)malloc(newtablesize * sizeof(unsigned int));
	if (table.it == nullptr) return ec::ec_alloc;
	memset(table.it, 0, newtablesize * sizeof(unsigned int));

	for (unsigned int i = 0; i < _tablesize; i++)
	{
		//Reading key offset
		unsigned int keyoffset = 0;
		ec code = _metaread(&keyoffset, i);
		if (code != ec::ec_ok) return code;
		
		if (keyoffset > 1)
		{
			//Reading key size
			unsigned int keysize = 0;
			unsigned int headersize = 0;
			ec code = _readsize(keyoffset, &keysize, &headersize);
			if (code != ec::ec_ok) return code;

			//Reading key
			void *readkey = nullptr;
			code = _readpointer(&readkey, keyoffset + headersize, keysize);
			if (code != ec::ec_ok) return code;

			//Calculate new hash
			unsigned int hash[4];
			ir::md5(readkey, keysize, hash);
			unsigned int index = hash[0] & (newtablesize - 1);

			//Inserting keyoffset to table
			while (true)
			{
				if (table.it[index] == 0) { table.it[index] = keyoffset; break; }
				else { index++; if (index == newtablesize) index = 0; }
			}
		}
	}

	//Write directly because I am tired
	if (_holdmeta)
	{
		free(_rammetafile);
		_rammetafile = table.it;
		table.it = nullptr;
	}
	else
	{
		if (fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;
		if (fwrite(table.it, sizeof(unsigned int), newtablesize, _metafile) < newtablesize) return ec::ec_write_file;
		_metapointer = newtablesize;
	}
	
	_delcount = 0;
	_tablesize = newtablesize;
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::directread(unsigned int index, ConstBlock *key, ConstBlock *data)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (key == nullptr && data == nullptr) return ec::ec_null;
	if (index >= _tablesize) return ec::ec_key_not_exists;

	//Find dataoffset
	unsigned int keyoffset = 0;
	ec code = _metaread(&keyoffset, index);
	if (code != ec::ec_ok) return code;
	if (keyoffset <= 1) return ec::ec_key_not_exists;

	//Read key size
	unsigned int keysize = 0;
	unsigned int keyheadersize = 0;
	code = _readsize(keyoffset, &keysize, &keyheadersize);
	if (code != ec::ec_ok) return code;

	//Read only key
	if (key != nullptr && data == nullptr)
	{
		void *readkey = nullptr;
		code = _readpointer(&readkey, keyoffset + keyheadersize, keysize);
		if (code != ec::ec_ok) return code;
		key->data = readkey;
		key->size = keysize;
		return ec::ec_ok;
	}

	//Read data size
	unsigned int dataoffset = keyoffset + keyheadersize + keysize;
	unsigned int datasize = 0;
	unsigned int dataheadersize = 0;
	code = _readsize(dataoffset, &datasize, &dataheadersize);
	if (code != ec::ec_ok) return code;

	if (key == nullptr)
	{
		//Read only data
		void *readdata = nullptr;
		code = _readpointer(&readdata, keyoffset + keyheadersize + keysize, datasize);
		if (code != ec::ec_ok) return code;
		data->data = readdata;
		data->size = datasize;
	}
	else
	{
		//Read key and data. May be optimized!
		void *readkeydata = nullptr;
		code = _readpointer(&readkeydata, keyoffset + keyheadersize, keysize + dataheadersize + datasize);
		if (code != ec::ec_ok) return code;
		key->data = readkeydata;
		key->size = keysize;
		data->data = (char*)readkeydata + keysize + dataheadersize;
		data->size = datasize;
	}

	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::read(ConstBlock key, ConstBlock *data)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (data == nullptr) return ec::ec_null;

	//Find dataoffset
	unsigned int dataoffset = 0;
	unsigned int index = 0;
	ec code = _find(key, &index, &dataoffset);
	if (code != ec::ec_ok) return code;
	if (dataoffset <= 1) return ec::ec_key_not_exists;

	//Read data size
	unsigned int datasize = 0;
	unsigned int headersize = 0;
	code = _readsize(dataoffset, &datasize, &headersize);
	if (code != ec::ec_ok) return code;
	
	//Read data
	void *readdata = nullptr;
	code = _readpointer(&readdata, dataoffset + headersize, datasize);
	data->data = readdata;
	data->size = datasize;

	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::insert(ConstBlock key, ConstBlock data, insertmode mode)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (!_writeaccess) return ec::ec_write_file;

	//Find dataoffset
	unsigned int olddataoffset = 0;
	unsigned int index = 0;
	ec code = _find(key, &index, &olddataoffset);
	if (code != ec::ec_ok) return code;
	
	if (mode == insertmode::insert_existing && olddataoffset <= 1) return ec::ec_key_not_exists;
	if (mode == insertmode::insert_not_existing && olddataoffset > 1) return ec::ec_key_already_exists;

	//Write key
	unsigned int newkeyoffset = 0;
	code = _writeblock(key, &newkeyoffset);
	if (code != ec::ec_ok) return code;
	
	//Write data
	unsigned int newdataoffset = 0;	//actually not needed
	code = _writeblock(data, &newdataoffset);
	if (code != ec::ec_ok) return code;

	//Write newkeyoffset to metafile
	code = _metawrite(newkeyoffset, index);
	if (code != ec::ec_ok) return code;

	if (olddataoffset == 0) _count++;
	else if (olddataoffset == 1) { _count++; _delcount--; }
	if (2 * (_count + _delcount) > _tablesize) return _rehash(2 * _tablesize);
	
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::delet(ConstBlock key, deletemode mode)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (!_writeaccess) return ec::ec_write_file;

	//Find dataoffset
	unsigned int olddataoffset = 0;
	unsigned int index = 0;
	ec code = _find(key, &index, &olddataoffset);
	if (code != ec::ec_ok) return code;

	if (olddataoffset <= 1)
	{
		if (mode == deletemode::delete_existing) return ec::ec_key_not_exists;
		else return ec::ec_ok;
	}

	//Write 1 or 0 to metafile
	unsigned int nextkeyoffset = 0;
	code = _metaread(&nextkeyoffset, (index + 1) & (_tablesize - 1));
	if (code != ec::ec_ok) return code;
	
	if (nextkeyoffset == 0)
	{
		code = _metawrite(0, index);
		if (code != ec::ec_ok) return code;
	}
	else
	{
		code = _metawrite(1, index);
		if (code != ec::ec_ok) return code;
		_delcount++;
	}
	_count--;

	return ec::ec_ok;
};

ir::S2STDatabase::~S2STDatabase()
{
	setrammode(false, false);
	if (_file != nullptr) fclose(_file);
	if (_metafile != nullptr)
	{
		if (_writeaccess)
		{
			fseek(_metafile, offsetof(MetaFileHeader, count), SEEK_SET);
			fwrite(&_count, sizeof(unsigned int), 1, _metafile);
			fseek(_metafile, offsetof(MetaFileHeader, delcount), SEEK_SET);
			fwrite(&_delcount, sizeof(unsigned int), 1, _metafile);
		}
		fclose(_metafile);
	}
};

#endif	//#ifndef IR_S2ST_DATABASE_IMPLEMENTATION