/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_DATABASE_TABLE_IMPLEMENTATION
#define IR_DATABASE_TABLE_IMPLEMENTATION

#include <malloc.h>
#include "ir_md5.h"

void *ir::TableDatabase::_openmap(unsigned int offset, unsigned int size, openmapmode mode)
{
	if (offset >= _headcache.highlimit) return openmap(&_bodycache, offset, size, mode);
	else return openmap(&_headcache, offset, size, _writeaccess ? openmapmode::openmap_readwrite : openmapmode::openmap_read);
};

bool ir::TableDatabase::_validsettings(const Settings *settings)
{
	return !((settings == nullptr)
		|| (settings->offsetsize > sizeof(unsigned int))
		|| (settings->offsetsize < sizeof(unsigned short int))
		|| (settings->tablesize == 0));
};

ir::ec ir::TableDatabase::_openfile(const Settings *settings)
{
	//Get file handle
	_hfile = CreateFileW(_path, GENERIC_READ | (_writeaccess ? GENERIC_WRITE : 0), FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (_hfile == INVALID_HANDLE_VALUE)	return ec::ec_open_file;

	_headcache.hfile = _bodycache.hfile = _hfile;

	//Get logical size (at the beginning is equal to physical)
	_filesize = GetFileSize(_hfile, nullptr);

	//Read header
	_FileHeader *header = (_FileHeader*)openmap(&_headcache, 0, sizeof(_FileHeader), openmapmode::openmap_read);
	if (header == nullptr) return ec::ec_openmap;

	//Compare signature
	if ((memcmp(header->signature, "IDT", 3) != 0) || (header->version != 0))
		return ec::ec_invalid_signature;

	//Check settings
	if (settings != nullptr)
	{
		if (!_validsettings(settings)) return ec::ec_database_invalid_settings;
	}
	else
	{
		if ((memcmp(&header->settings, settings, sizeof(Settings)) != 0) || !_validsettings(settings)) return ec::ec_database_invalid_settings;
	}
	_settings = header->settings;

	//Map the head region
	void *headmap = openmap(&_headcache, sizeof(_FileHeader), _settings.tablesize * _settings.offsetsize, _writeaccess ? openmapmode::openmap_readwrite : openmapmode::openmap_read);
	if (headmap == nullptr) return ec::ec_openmap;
	
	return ec::ec_ok;
};

ir::ec ir::TableDatabase::_createfile(const Settings *settings)
{
	//Check settings
	if (!_validsettings(settings)) return ec::ec_database_invalid_settings;
	_settings = *settings;

	//Get file handle
	_hfile = CreateFileW(_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);
	if (_hfile == INVALID_HANDLE_VALUE)	return ec::ec_open_file;

	_headcache.hfile = _bodycache.hfile = _hfile;

	//Write header
	_FileHeader *header = (_FileHeader*)openmap(&_headcache, 0, sizeof(_FileHeader), openmapmode::openmap_write);
	memcpy(header->signature, "IDT", 3 * sizeof(char));
	header->version = 0;
	memcpy(&header->settings, settings, sizeof(Settings));
	header->count = 0;

	_filesize = sizeof(_FileHeader) + _settings.tablesize * _settings.offsetsize;

	//Map the head region
	void *writepointer = openmap(&_headcache, sizeof(_FileHeader), _settings.tablesize * _settings.offsetsize, openmapmode::openmap_readwrite);
	if (writepointer == nullptr) return ec::ec_openmap;
	memset(writepointer, 0, _settings.tablesize * _settings.offsetsize);

	return ec::ec_ok;
};

unsigned int ir::TableDatabase::_makemarker(unsigned int size, unsigned char marker[5], bool key)
{
	//If has constant key/data size -> do nothings
	if ((key && (_settings.constkeysize != 0)) || (!key && (_settings.constdatasize != 0)))
		return 0;

	if (size < (256 - 4))
	{
		marker[0] = size;
		return 1;
	}
	else
	{
		unsigned int bytes = 0;
		for (unsigned int buffersize = size; buffersize != 0; buffersize >>= 8)	bytes++;

		marker[0] = bytes + (256 - 4) - 1;
		memcpy(&marker[1], &size, bytes);
		return bytes + 1;
	}
};

ir::ec ir::TableDatabase::_readmarker(unsigned int *offset, unsigned int *size, bool key)
{
	if (key && (_settings.constkeysize != 0))
	{
		*size = _settings.constkeysize;
		return ec::ec_ok;
	}
	else if (!key && (_settings.constdatasize != 0))
	{
		*size = _settings.constdatasize;
		return ec::ec_ok;
	}
	else
	{
		unsigned char *marker = (unsigned char*)_openmap(*offset, 1, openmapmode::openmap_read);
		if (marker == nullptr) return ec::ec_openmap;

		unsigned char smallsize = *marker;
		if (smallsize < (256 - 4))
		{
			*size = smallsize;
			*offset = *offset + 1;
		}
		else
		{
			smallsize = smallsize - (256 - 4) + 1;													//*offset points to [quantity of bytes]
			marker = (unsigned char*)_openmap(*offset + 1, smallsize, openmapmode::openmap_read);	//*offset + 1 points to [bytes]
			if (marker == nullptr) return ec::ec_openmap;

			memcpy(size, marker, smallsize);														//[bytes] are size of data
			*offset = *offset + 1 + smallsize;														//*offset + 1 + smallsize points to start of data
		}
	}

	return ec::ec_ok;
}

ir::ec ir::TableDatabase::_keymatch(unsigned int *offset, const Data *key, bool *match)
{
	//Read size of the key
	Data readdata;
	ir::ec code = _readmarker(offset, &readdata.size, true);
	if (code != ec::ec_ok) return code;

	//Read data and compare if neccessary
	if (readdata.size == key->size)
	{
		readdata.data = _openmap(*offset, readdata.size, openmapmode::openmap_read);
		if (readdata.data == nullptr) return ec::ec_openmap;
		*match = (memcmp(readdata.data, key->data, readdata.size) == 0);
	}
	else *match = false;

	*offset = *offset + readdata.size;
	return ec::ec_ok;
};

ir::ec ir::TableDatabase::_rewriteoffset(unsigned int *offset, unsigned int offsetoffset)
{
	//Read offset of the next entry
	Data readdata;
	readdata.data = _openmap(*offset, _settings.offsetsize, openmapmode::openmap_read);
	if (readdata.data == nullptr) return ec::ec_openmap;
	*offset = 0;
	memcpy(offset, readdata.data, _settings.offsetsize);

	//Write offset to the current entry
	void *writepointer = _openmap(offsetoffset, _settings.offsetsize, openmapmode::openmap_write);
	if (writepointer == nullptr) return ec::ec_openmap;
	memcpy(writepointer, offset, _settings.offsetsize);

	return ec::ec_ok;
};

ir::ec ir::TableDatabase::_calculateindex(Data key, unsigned int *index)
{
	if (_settings.mode == tablemode::chain)
	{
		unsigned int hash[4];
		ir::md5(key.data, key.size, hash);
		if ((_settings.tablesize & (_settings.tablesize - 1)) == 0) *index = hash[0] & (_settings.tablesize - 1);
		else *index = hash[0] / _settings.tablesize;
	}
	else
	{
		unsigned int i = 0;
		if (key.size > sizeof(unsigned int)) return ec::ec_database_overflow;
		memcpy(&i, key.data, key.size);
		if (i >= _settings.tablesize) return ec::ec_database_overflow;
		*index = i;
	}
	return ec::ec_ok;
};

unsigned int ir::TableDatabase::count()
{
	return _ok? _count : 0;
};

unsigned int ir::TableDatabase::filesize()
{
	return _ok? _filesize : 0;
};

ir::TableDatabase::TableDatabase(const wchar_t* path, const Settings *settings, bool writeaccess, ir::ec *code)
{
	_writeaccess = writeaccess;
	
	DWORD attributes = GetFileAttributesW(path);
	if ((attributes != INVALID_FILE_ATTRIBUTES) && ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
	{
		*code = ec::ec_path_is_directory;
		return;
	}

	_path = _wcsdup(path);
	if (_path == nullptr)
	{
		*code = ec::ec_alloc;
		return;
	}

	//Open/create file
	*code = (attributes == INVALID_FILE_ATTRIBUTES) ? _createfile(settings) : _openfile(settings);
	if (*code == ec::ec_ok) _ok = true;
};

ir::TableDatabase::~TableDatabase()
{
	if (_hfile != INVALID_HANDLE_VALUE)
	{		
		if (_writeaccess)
		{
			void *writepointer = _openmap(offsetof(ir::TableDatabase::_FileHeader, count), sizeof(unsigned int), openmapmode::openmap_write);
			if (writepointer != nullptr) *((unsigned int*)writepointer) = _count;
			openmap(&_headcache, 0, 0, openmapmode::openmap_no);
			openmap(&_bodycache, 0, 0, openmapmode::openmap_no);
			SetFilePointer(_hfile, (LONG)_filesize, nullptr, FILE_BEGIN);
			SetEndOfFile(_hfile);
		}
		else
		{
			openmap(&_headcache, 0, 0, openmapmode::openmap_no);
			openmap(&_bodycache, 0, 0, openmapmode::openmap_no);
		}
		CloseHandle(_hfile);
	}

	if (_path != nullptr) free(_path);
};

ir::ec ir::TableDatabase::read(Data key, Data *data)
{
	if (!_ok) return ec::ec_database_not_ok;
	if ((_settings.constkeysize != 0) && (_settings.constkeysize != key.size)) return ec::ec_database_invalid_size;
	if (data == nullptr) return ec::ec_null;
	if ((key.size != 0) && (key.data == nullptr)) return ec::ec_null;

	//Calculating index
	unsigned int index;
	ir::ec code = _calculateindex(key, &index);
	if (code != ec::ec_ok) return code;
	
	//Reading offset of the first entry in queue
	unsigned int offset = 0;
	const void *readpointer = _openmap(sizeof(_FileHeader) + _settings.offsetsize * index, _settings.offsetsize, openmapmode::openmap_read);
	if (readpointer == nullptr) return ec::ec_openmap;
	memcpy(&offset, readpointer, _settings.offsetsize);

	while (offset != 0)
	{
		//Check for key match
		bool match;
		code = _keymatch(&offset, &key, &match);
		if (code != ec::ec_ok) return code;
		
		//Read size of data of the entry
		Data readdata;
		code = _readmarker(&offset, &readdata.size, false);
		if (code != ec::ec_ok) return code;

		//If match -> read the data and return
		if (match)
		{
			//Read the data
			readdata.data = _openmap(offset, readdata.size, openmapmode::openmap_read);
			if (readdata.data == nullptr) return ec::ec_openmap;

			*data = readdata;
			return ec::ec_ok;
		}
		//Else skip data
		else offset += readdata.size;

		//Read offset to the next entry (never happens with checkmatch == false)
		const void *readpointer = _openmap(offset, _settings.offsetsize, openmapmode::openmap_read);
		if (readpointer == nullptr) return ec::ec_openmap;
		offset = 0;
		memcpy(&offset, readpointer, _settings.offsetsize);
	}

	return ec::ec_database_not_exists;
};

ir::ec ir::TableDatabase::insert(Data key, Data data, insertmode mode)
{
	if (!_ok) return ec::ec_database_not_ok;
	if (!_writeaccess) return ec::ec_database_no_write_access;
	if ((_settings.constkeysize != 0) && (_settings.constkeysize != key.size)) return ec::ec_database_invalid_size;
	if ((_settings.constdatasize != 0) && (_settings.constdatasize != data.size)) return ec::ec_database_invalid_size;
	if ((key.size != 0) && (key.data == nullptr)) return ec::ec_null;


	unsigned int dc = 0; //DEBUG

	//Calculating index
	unsigned int index;
	ir::ec code = _calculateindex(key, &index);
	if (code != ec::ec_ok) return code;

	//Reading offset of the first entry in queue
	bool found = false;
	unsigned int offsetoffset = sizeof(_FileHeader) + _settings.offsetsize * index;
	unsigned int offset = 0;
	const void *readpointer = _openmap(offsetoffset, _settings.offsetsize, openmapmode::openmap_read);
	if (readpointer == nullptr) return ec::ec_openmap;
	memcpy(&offset, readpointer, _settings.offsetsize);

	while (offset != 0)
	{
		dc++;
		if (dc > 100)
		{
			dc = 0;
		}

		//Check for key match
		bool match;
		code = _keymatch(&offset, &key, &match);
		if (code != ec::ec_ok) return code;

		//Skip data
		unsigned int datasize;
		code = _readmarker(&offset, &datasize, false);
		if (code != ec::ec_ok) return code;
		offset += datasize;

		//If match -> write next offset to previous offset (delete)
		if (match)
		{
			if (mode == insert_not_existing) return ec::ec_database_already_exists;
			found = true;

			code = _rewriteoffset(&offset, offsetoffset);
			if (code != ec::ec_ok) return code;

			_count--;
		}
		else
		//If no match -> read offest of the next entry (never happens for checkmatch == false)
		{
			offsetoffset = offset;
			const void *readpointer = _openmap(offset, _settings.offsetsize, openmapmode::openmap_read);
			if (readpointer == nullptr) return ec::ec_openmap;
			offset = 0;
			memcpy(&offset, readpointer, _settings.offsetsize);
		}

		if (offset > 100000000)
		{
			match = 0;
		}
	}

	if ((mode == insert_existing) && !found) return ec::ec_database_not_exists;
	
	//Adding new entry:
	unsigned char keymarker[5], datamarker[5];
	char keymarkersize = _makemarker(key.size, keymarker, true);
	char datamarkersize = _makemarker(data.size, datamarker, false);
	unsigned int entrysize = keymarkersize + key.size + _settings.offsetsize + datamarkersize + data.size;

	//If in head is not yet full, but the entry does not fit in head -> start writing body
	if ((_filesize < _headcache.highlimit) && (_filesize + entrysize > _headcache.highlimit))
	{
		offset = _headcache.highlimit;
	}
	else
	{
		offset = _filesize;
	}

	void *writepointer = _openmap(offset, entrysize, openmapmode::openmap_write);
	if (writepointer == nullptr) return ec::ec_openmap;
	
	//Write key:
	memcpy(writepointer, keymarker, keymarkersize);
	writepointer = (char*)writepointer + keymarkersize;
	memcpy(writepointer, key.data, key.size);
	writepointer = (char*)writepointer + key.size;

	//Write data:
	memcpy(writepointer, datamarker, datamarkersize);
	writepointer = (char*)writepointer + datamarkersize;
	memcpy(writepointer, data.data, data.size);
	writepointer = (char*)writepointer + data.size;
	
	//Write NULL offset
	memset(writepointer, 0, _settings.offsetsize);

	//Write offset of the entry into the previous entry (or into the table)
	writepointer = _openmap(offsetoffset, _settings.offsetsize, openmapmode::openmap_write);
	if (writepointer == nullptr) return ec::ec_openmap;
	memcpy(writepointer, &_filesize, _settings.offsetsize);

	_filesize = offset + entrysize;

	_count++;
	return ec::ec_ok;
};

ir::ec ir::TableDatabase::delet(Data key, deletemode mode)
{
	if (!_ok) return ec::ec_database_not_ok;
	if (!_writeaccess) return ec::ec_database_no_write_access;
	if ((_settings.constkeysize != 0) && (_settings.constkeysize != key.size)) return ec::ec_database_invalid_size;
	if ((key.size != 0) && (key.data == nullptr)) return ec::ec_null;

	//Calculating index
	unsigned int index;
	ir::ec code = _calculateindex(key, &index);
	if (code != ec::ec_ok) return code;

	//Reading offset of the first entry in queue
	unsigned int offsetoffset = sizeof(_FileHeader) + _settings.offsetsize * index;
	unsigned int offset = 0;
	const void *readpointer = _openmap(offsetoffset, _settings.offsetsize, openmapmode::openmap_read);
	if (readpointer == nullptr) return ec::ec_openmap;
	memcpy(&offset, readpointer, _settings.offsetsize);

	while (true)
	{
		//Check for key match
		bool match;
		code = _keymatch(&offset, &key, &match);
		if (code != ec::ec_ok) return code;

		//Skip data
		unsigned int readdatasize;
		code = _readmarker(&offset, &readdatasize, false);
		if (code != ec::ec_ok) return code;
		offset += readdatasize;

		//If match -> write next offset to previous offset (delete)
		if (match)
		{
			code = _rewriteoffset(&offset, offsetoffset);
			if (code != ec::ec_ok) return code;

			_count--;
			return ec::ec_ok;
		}
		//If no match -> go to next entry or exit
		else
		{
			offsetoffset = offset;
			const void *readpointer = _openmap(offset, _settings.offsetsize, openmapmode::openmap_read);
			if (readpointer == nullptr) return ec::ec_openmap;
			offset = 0;
			memcpy(&offset, readpointer, _settings.offsetsize);
		}
	}

	if (mode == delete_always) return ec::ec_ok;
	else return ec::ec_database_not_exists;
};

#endif //#ifndef IR_DATABASE_TABLE_IMPLEMENTATION