#ifndef IR_T_DATABASE_IMPLEMENTATION
#define IR_T_DATABASE_IMPLEMENTATION

#include <ir_reserve.h>

ir::ec ir::TDatabase::_read(void *buffer, unsigned int offset, unsigned int size)
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

ir::ec ir::TDatabase::_write(const void *buffer, unsigned int offset, unsigned int size)
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

ir::ec ir::TDatabase::_readpointer(void **p, unsigned int offset, unsigned int size)
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
		void *pointer = openmap(&_mapcache, _file.file, offset, size, openmapmode::openmap_read);
		if (pointer == nullptr) return ec::ec_openmap;
		memcpy(p, &pointer, sizeof(void*));
	}
	return ec::ec_ok;
};

ir::ec ir::TDatabase::_metaread(unsigned int *keyoffset, unsigned int index)
{
	if (index >= _meta.size) return ec::ec_read_file;

	if (_meta.hold)
	{
		*keyoffset = _meta.ram[index];
	}
	else
	{
		if (index != _meta.pointer)
		{
			if (fseek(_meta.file, _metaheadersize + index * sizeof(unsigned int), SEEK_SET) != 0) return ec::ec_seek_file;
			_meta.pointer = index;
		}
		if (fread(keyoffset, sizeof(unsigned int), 1, _meta.file) == 0) return ec::ec_read_file;
		_meta.pointer++;
	}
	return ec::ec_ok;
};

//0 - 249	- block size directrly
//250		- next unsigned int is block size
//251 - 254	- next 1/2/3/4 bytes are reserbed
//255		- next unsigned int is reserved block size
//if not tight, single unsigned int evetywhere, first bit is for reserved
unsigned int ir::TDatabase::_headersize(unsigned int size)
{
	if (_tight)
	{
		if (size > 249) return sizeof(unsigned char) + sizeof(unsigned int);
		else return sizeof(unsigned char);
	}
	else return sizeof(unsigned int);
};

ir::ec ir::TDatabase::_readheader(unsigned int headeroffset, InternalHeader *header)
{
	if (_tight)
	{
		unsigned char marker = 0;
		ec code = _read(&marker, headeroffset, sizeof(unsigned char));
		if (code != ec::ec_ok) return code;
		
		if (marker < 250)
		{
			header->size = marker;
			header->headersize = sizeof(unsigned char);
			header->reserved = false;
		}
		else if (marker == 250)
		{
			code = _read(&header->size, headeroffset + sizeof(unsigned char), sizeof(unsigned int));
			if (code != ec::ec_ok) return code;
			header->headersize = sizeof(unsigned char) + sizeof(unsigned int);
			header->reserved = false;
		}
		else if (marker < 255)
		{
			header->size = marker - 251;
			header->headersize = sizeof(unsigned char);
			header->reserved = true;
		}
		else
		{
			code = _read(&header->size, headeroffset + 1, sizeof(unsigned int));
			if (code != ec::ec_ok) return code;
			header->headersize = sizeof(unsigned char) + sizeof(unsigned int);
			header->reserved = true;
		}
	}
	else
	{
		ec code = _read(&header->size, headeroffset, sizeof(unsigned int));
		if (code != ec::ec_ok) return code;

		if (header->size & 0x80000000 != 0)
		{
			header->size &= 0x7FFFFFFF;
			header->reserved = true;
		}
		else header->reserved = false;
		header->headersize = sizeof(unsigned int);
	}
	return ec::ec_ok;
};

ir::ec ir::TDatabase::setrammode(bool holdfile, bool holdmeta)
{
	if (!_ok) return ec::ec_object_not_ok;

	//Read meta
	if (holdmeta && !_meta.hold)
	{
		_meta.ram = (unsigned int *)malloc(_meta.size * sizeof(unsigned int));
		if (_meta.ram == nullptr) return ec::ec_alloc;
		if (fseek(_meta.file, _metaheadersize, SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fread(_meta.ram, sizeof(unsigned int), _meta.size, _meta.file) < _meta.size) return ec::ec_read_file;
		_meta.pointer = _meta.size;
	}
	//Write meta
	else if (!holdmeta && _meta.hold)
	{
		if (_writeaccess && _meta.changed)
		{
			if (fseek(_meta.file, _metaheadersize, SEEK_SET) != 0) return ec::ec_seek_file;
			if (fwrite(_meta.ram, sizeof(unsigned int), _meta.size, _meta.file) < _meta.size) return ec::ec_write_file;
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
		_file.ram = malloc(_file.size * sizeof(unsigned int));
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

unsigned int ir::TDatabase::count()
{
	if (!_ok) return 0;
	else return _count;
};

unsigned int ir::TDatabase::tablesize()
{
	if (!_ok) return 0;
	else return _meta.size;
};

unsigned int ir::TDatabase::filesize()
{
	if (!_ok) return 0;
	else return _file.size;
};

#endif	//#ifndef IR_T_DATABASE_IMPLEMENTATION