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

class MemFreer{ public: static void free(void *mem) { ::free(mem); } };
typedef ir::Resource<ir::syschar*, MemFreer, nullptr> StringResource;
typedef ir::Resource<unsigned int*, MemFreer, nullptr> UintResource;

ir::ec ir::S2STDatabase::_read(void *buffer, unsigned int offset, unsigned int size)
{
	if (offset + size > _filesize) return ec::ec_read_file;
	
	if (_rammode == rammode::rammode_all)
	{
		memcpy(buffer, (char*)_ramfile + offset, size);
	}
	else
	{
		if (offset != _filepointer)
		{
			if (fseek(_file, offset, SEEK_SET) != 0) return ec::ec_seek_file;
			_filepointer = offset;
		}
		if (fread(buffer, size, 1, _file) == 0) return ec::ec_read_file;
		_filepointer += size;
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_write(const void *buffer, unsigned int offset, unsigned int size)
{
	if (_rammode == rammode::rammode_all)
	{
		if (offset + size > _filesize)
		{
			if (!reserve(&_ramfile, &_filesize, offset + size)) return ec::ec_alloc;
		};
		memcpy((char*)_ramfile + offset, buffer, size);
	}	
	else
	{
		if (offset != _filepointer)
		{
			if (fseek(_file, offset, SEEK_SET) != 0) return ec::ec_seek_file;
			_filepointer = offset;
		}
		if (fwrite(buffer, size, 1, _file) == 0) return ec::ec_read_file;
		_filepointer += size;
		if (offset + size > _filesize) _filesize = offset + size;
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_readpointer(void **p, unsigned int offset, unsigned int size)
{
	if (offset + size > _filesize) return ec::ec_read_file;

	if (_rammode == rammode::rammode_all)
	{
		void *pointer = (char*)_ramfile + offset;
		memcpy(p, &pointer, sizeof(void*));
	}
	else
	{
		if (offset != _filepointer)
		{
			if (fseek(_file, offset, SEEK_SET) != 0) return ec::ec_seek_file;
			_filepointer = offset;
		}
		if (!reserve(&_buffer.data, &_buffer.size, size)) return ec::ec_alloc;
		if (fread(_buffer.data, size, 1, _file) == 0) return ec::ec_read_file;
		_filepointer += size;
		memcpy(p, &_buffer.data, sizeof(void*));
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_metaread(unsigned int *keyoffset, unsigned int index)
{
	if (index >= _filesize) return ec::ec_read_file;

	if (_rammode == rammode::rammode_all)
	{
		*keyoffset = _rammetafile[index];
	}
	else
	{
		if (index != _metapointer)
		{
			if (fseek(_metafile, sizeof(MetaFileHeader) + index * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
			_metapointer = index;
		}
		if (fread(keyoffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_read_file;
		_metapointer++;
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_metawrite(unsigned int keyoffset, unsigned int index)
{
	if (index >= _filesize) return ec::ec_read_file;

	if (_rammode == rammode::rammode_all)
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

ir::ec ir::S2STDatabase::_readsize(unsigned int offset, unsigned int *size, unsigned int *headersize)
{
	ec code = _read(size, offset, sizeof(char));
	if (code != ec::ec_ok) return code;
	if (*size > 251)
	{
		*headersize = (*size - 251) + 1;
		code = _read(size, offset + 1, *size - 251);
		if (code != ec::ec_ok) return code;
	}
	else
	{
		*headersize = 1;
	}
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_writeblock(ConstBlock data, unsigned int *offset)
{
	*offset = _filesize;
	if (data.size > 251)
	{
		unsigned char sizeoff;
		if (data.size > 256 * 256 * 256) sizeoff = 4;
		else if (data.size > 256 * 256) sizeoff = 3;
		else if (data.size > 256) sizeoff = 2;
		else sizeoff = 1;

		unsigned int sizeoftowrite = sizeoff + 251;
		ec code = _write(&sizeoftowrite, _filesize, sizeof(char));
		if (code != ec::ec_ok) return code;
		code = _write(&data.size, _filesize, sizeoff);
		if (code != ec::ec_ok) return code;
	}
	else
	{
		if (_write(&data.size, _filesize, sizeof(char)) == 0) return ec::ec_write_file;
	}

	return _write(data.data, _filesize, data.size);
};

ir::ec ir::S2STDatabase::_checkfile()
{
	FileHeader header;
	if (fseek(_file, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&header, sizeof(FileHeader), 1, _file) == 0				||
		memcmp(header.signature, "IS2STDF", 7) != 0						||
		header.version != 0) return ec::ec_invalid_signature;
	
	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_checkmetafile()
{
	MetaFileHeader metaheader;
	if (fseek(_metafile, 0, SEEK_SET) != 0) return ec::ec_seek_file;
	if (fread(&metaheader, sizeof(MetaFileHeader), 1, _metafile) == 0	||
		memcmp(metaheader.signature, "IS2STDM", 7) != 0					||
		metaheader.version != 0) return ec::ec_invalid_signature;
	
	_count = metaheader.count;
	if (fseek(_metafile, 0, SEEK_END) != 0) return ec::ec_seek_file;
	_tablesize = ftell(_metafile) - sizeof(MetaFileHeader);
	if (_tablesize < sizeof(unsigned int) || (_tablesize & (_tablesize - 1) == 0)) return ec::ec_invalid_signature;
	_tablesize /= sizeof(unsigned int);

	return ec::ec_ok;
};

ir::ec ir::S2STDatabase::_openfile(const syschar *filepath, bool meta, createmode cmode)
{
	FILE **file = meta ? &_metafile : &_file;
	*file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	ec check = ec::ec_ok;
	bool exists = false;
	
	if (*file != nullptr)
	{
		exists = true;
		check = meta ? _checkmetafile() : _checkfile();
	}

	bool createnew;
	switch (cmode)
	{
	case createmode::readonly:
		if (!exists) return ec::ec_open_file;
		else if (check != ec::ec_ok) return check;
		else return ec::ec_ok;
		break;
	case createmode::new_never:
		if (!exists) return ec::ec_open_file;
		else if (check != ec::ec_ok) return check;
		else createnew = false;
		break;
	case createmode::new_if_no:
		if (!exists) createnew = true;
		else if (check != ec::ec_ok) return check;
		else createnew = false;
		break;
	case createmode::new_if_corrupted:
		if (!exists) return ec::ec_open_file;
		else if (check != ec::ec_ok) createnew = true;
		else createnew = false;
		break;
	case createmode::new_if_no_or_corrupted:
		if (!exists) createnew = true;
		if (check != ec::ec_ok) createnew = true;
		createnew = false;
		break;
	case createmode::new_always:
		createnew = true;
		break;
	}

	if (*file != nullptr) fclose(*file);
	*file = nullptr;

	if (createnew)
	{
		*file = _wfsopen(filepath, L"wb", _SH_DENYNO);
		if (*file == nullptr) return ec::ec_create_file;
		if (meta)
		{
			MetaFileHeader header;
			if (fwrite(&header, sizeof(MetaFileHeader), 1, *file) == 0) return ec::ec_write_file;
			_metapointer = 0;
			_count = 0;
			_delcount = 0;
		}
		else
		{
			FileHeader header;
			if (fwrite(&header, sizeof(FileHeader), 1, *file) == 0) return ec::ec_write_file;
			_filepointer = sizeof(FileHeader);
		}
	}
	else
	{
		*file = _wfsopen(filepath, L"w+b", _SH_DENYNO);
		if (*file == nullptr) return ec::ec_create_file;
	}
}

ir::ec ir::S2STDatabase::_init(const syschar *filepath, createmode cmode, rammode rmode)
{
	#ifdef _WIN32
		unsigned int pathlen = wcslen(filepath);
	#else
		unsigned int pathlen = strlen(filepath);
	#endif
	
	StringResource metafilepath = (syschar*)malloc((pathlen + 2) * sizeof(syschar));
	if (metafilepath.it == nullptr) return ec::ec_alloc;
	memcpy(metafilepath.it, filepath, pathlen * sizeof(syschar));
	metafilepath.it[pathlen] = '~';
	metafilepath.it[pathlen + 1] = '\0';
	
	ec code = _openfile(filepath, false, cmode);
	if (code != ec::ec_ok) return code;

	code = _openfile(metafilepath.it, true, cmode);
	if (code != ec::ec_ok) return code;

	code = setrammode(rmode);
	if (code != ec::ec_ok) return code;

	_ok = true;
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
			unsigned int keysize, headersize;
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
	if (_rammode == rammode::rammode_no)
	{
		if (fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;
		if (fwrite(table.it, sizeof(unsigned int), newtablesize, _metafile) < newtablesize) return ec::ec_write_file;
	}
	else
	{
		free(_rammetafile);
		_rammetafile = table.it;
		table.it = nullptr;
	}
	
	_delcount = 0;
	_tablesize = newtablesize;
	return ec::ec_ok;
};

ir::S2STDatabase::S2STDatabase(const syschar *filepath, createmode cmode, rammode rmode, ec *code)
{
	ec c = _init(filepath, cmode, rmode);
	if (code != nullptr) *code = c;
};

unsigned int ir::S2STDatabase::count()
{
	if (!_ok) return 0;
	else return _count;
};

unsigned int ir::S2STDatabase::tablesize()
{
	if (!_ok) return 0;
	else return _tablesize;
};

ir::ec ir::S2STDatabase::setrammode(rammode rmode)
{
	//Read meta
	if (_rammode == rammode::rammode_no && rmode != rammode::rammode_no)
	{
		_rammetafile = (unsigned int *)malloc(_tablesize * sizeof(unsigned int));
		fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET);
		fread(_rammetafile, _tablesize, sizeof(unsigned int), _metafile);
	}
	//Write meta
	else if (_rammode != rammode::rammode_no && rmode == rammode::rammode_no)
	{
		fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET);
		fwrite(_rammetafile, _tablesize, sizeof(unsigned int), _metafile);
		free(_rammetafile);
		_rammetafile = nullptr;
	}

	//Read data
	if (_rammode != rammode::rammode_all && rmode == rammode::rammode_all)
	{
		_ramfile = malloc(_filesize * sizeof(unsigned int));
		fseek(_file, sizeof(MetaFileHeader), SEEK_SET);
		fread(_ramfile, _filesize, sizeof(unsigned int), _file);
	}
	//Write data
	else if (_rammode == rammode::rammode_all && rmode != rammode::rammode_all)
	{
		fseek(_file, sizeof(MetaFileHeader), SEEK_SET);
		fwrite(_ramfile, _filesize, sizeof(unsigned int), _file);
		free(_ramfile);
		_ramfile = nullptr;
	}

	_rammode == rmode;
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

	//Find dataoffset
	unsigned int olddataoffset = 0;
	unsigned int index = 0;
	ec code = _find(key, &index, &olddataoffset);
	if (code != ec::ec_ok) return code;
	
	if (mode == insertmode::insert_existing && olddataoffset > 1) return ec::ec_key_not_exists;
	if (mode == insertmode::insert_not_existing && olddataoffset <= 1) return ec::ec_key_already_exists;

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

	//Write 1 to metafile
	if (fseek(_metafile, index, SEEK_SET) != 0) return ec::ec_seek_file;
	unsigned int newkeyoffset = 1;	//Optimize! May write 0 if next if 0!
	if (fwrite(&newkeyoffset, sizeof(unsigned int), 1, _metafile) == 0) return ec::ec_write_file;

	_count--;
	_delcount++;
	return ec::ec_ok;
};

ir::S2STDatabase::~S2STDatabase()
{
	setrammode(rammode::rammode_no);
	if (_file != nullptr) fclose(_file);
	if (_metafile != nullptr)
	{
		fseek(_metafile, offsetof(MetaFileHeader, count), SEEK_SET);
		fwrite(&_count, sizeof(unsigned int), 1, _metafile);
		fseek(_metafile, offsetof(MetaFileHeader, delcount), SEEK_SET);
		fwrite(&_delcount, sizeof(unsigned int), 1, _metafile);
		fclose(_metafile);
	}
};

#endif	//#ifndef IR_S2ST_DATABASE_IMPLEMENTATION