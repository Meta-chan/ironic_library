#ifndef IR_MEMFREES
#define IR_MEMFREER
	//I really don't know how to make templates part of Ironic. Maybe later
	class MemFreer{ public: static void free(void *mem) { ::free(mem); } };
	typedef ir::Resource<ir::syschar*, MemFreer, nullptr> StringResource;
	typedef ir::Resource<unsigned int*, MemFreer, nullptr> UintResource;
#endif

ir::ec ir::IR_T_DATABASE_TYPE::_read(void *buffer, unsigned int offset, unsigned int size)
{
	if (offset + size > _filesize) return ec::ec_read_file;
	
	if (_holdfile)
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

ir::ec ir::IR_T_DATABASE_TYPE::_write(const void *buffer, unsigned int offset, unsigned int size)
{
	if (_holdfile)
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

ir::ec ir::IR_T_DATABASE_TYPE::_readpointer(void **p, unsigned int offset, unsigned int size)
{
	if (offset + size > _filesize) return ec::ec_read_file;

	if (_holdfile)
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

ir::ec ir::IR_T_DATABASE_TYPE::_metaread(unsigned int *keyoffset, unsigned int index)
{
	if (index >= _tablesize) return ec::ec_read_file;

	if (_holdmeta)
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

ir::ec ir::IR_T_DATABASE_TYPE::_readsize(unsigned int offset, unsigned int *size, unsigned int *headersize)
{
	*size = 0;
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

ir::ec ir::IR_T_DATABASE_TYPE::_writeblock(ConstBlock data, unsigned int *offset)
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
		ec code = _write(&data.size, _filesize, sizeof(char));
		if (code != ec::ec_ok) return code;
	}

	return _write(data.data, _filesize, data.size);
};

ir::ec ir::IR_T_DATABASE_TYPE::_init(const syschar *filepath, createmode cmode)
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
	
	ec filestatus = _checkfile(filepath);
	ec metastatus = _checkmetafile(metafilepath.it);
	
	if (filestatus == ec::ec_ok && metastatus == ec::ec_ok) filestatus = ec::ec_ok;
	else if (filestatus == ec::ec_open_file && metastatus == ec::ec_open_file) filestatus = ec::ec_open_file;
	else filestatus = ec::ec_invalid_signature;

	//Hehehehe. I am sorry(
	ec code = ec::ec_ok;
	switch (cmode)
	{
	case createmode::create_readonly:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus != ec::ec_ok) return ec::ec_invalid_signature;
		else return ec::ec_ok;
		break;
	case createmode::create_new_never:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus == ec::ec_invalid_signature) return ec::ec_invalid_signature;
		else code = _openwrite(filepath, metafilepath.it, false);
		break;
	case createmode::create_new_if_no:
		if (filestatus == ec::ec_open_file) code = _openwrite(filepath, metafilepath.it, true);
		else if (filestatus == ec::ec_invalid_signature) return ec::ec_invalid_signature;
		else code = _openwrite(filepath, metafilepath.it, false);
		break;
	case createmode::create_new_if_corrupted:
		if (filestatus == ec::ec_open_file) return ec::ec_open_file;
		else if (filestatus == ec::ec_invalid_signature) code = _openwrite(filepath, metafilepath.it, true);
		else code = _openwrite(filepath, metafilepath.it, false);
		break;
	case createmode::create_new_if_no_or_corrupted:
		if (filestatus == ec::ec_open_file) code = _openwrite(filepath, metafilepath.it, true);
		if (filestatus == ec::ec_invalid_signature) code = _openwrite(filepath, metafilepath.it, true);
		code = _openwrite(filepath, metafilepath.it, false);
		break;
	case createmode::create_new_always:
		code = _openwrite(filepath, metafilepath.it, true);
		break;
	}

	if (code != ec::ec_ok) return code;
	_ok = true;
	return ec::ec_ok;
};

ir::ec ir::IR_T_DATABASE_TYPE::setrammode(bool holdfile, bool holdmeta)
{
	if (!_ok) return ec::ec_object_not_ok;

	//Read meta
	if (holdmeta && !_holdmeta)
	{
		_rammetafile = (unsigned int *)malloc(_tablesize * sizeof(unsigned int));
		if (_rammetafile == nullptr) return ec::ec_alloc;
		if (fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fread(_rammetafile, _tablesize, sizeof(unsigned int), _metafile) < _tablesize) return ec::ec_read_file;
		_metapointer = _tablesize;
	}
	//Write meta
	else if (!holdmeta && _holdmeta)
	{
		if (fseek(_metafile, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;
		if (fwrite(_rammetafile, _tablesize, sizeof(unsigned int), _metafile) < _tablesize) return ec::ec_write_file;
		free(_rammetafile);
		_rammetafile = nullptr;
		_metapointer = _tablesize;
	}

	//Read data
	if (holdfile && !_holdfile)
	{
		_ramfile = malloc(_filesize * sizeof(unsigned int));
		if (_ramfile == nullptr) return ec::ec_alloc;
		if (fseek(_file, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fread(_ramfile, _filesize, 1, _file) < _filesize) return ec::ec_read_file;;
		_filepointer = _filesize;
	}
	//Write data
	else if (!holdfile && _holdfile)
	{
		if (fseek(_file, sizeof(MetaFileHeader), SEEK_SET) != 0) return ec::ec_seek_file;;
		if (fwrite(_ramfile, _filesize, 1, _file) < _filesize) return ec::ec_write_file;
		free(_ramfile);
		_ramfile = nullptr;
		_filepointer = _filesize;
	}
	
	_holdfile = holdfile;
	_holdmeta = holdmeta;

	return ec::ec_ok;
};

ir::IR_T_DATABASE_TYPE::IR_T_DATABASE_TYPE(const syschar *filepath, createmode cmode, ec *code)
{
	ec c = _init(filepath, cmode);
	if (code != nullptr) *code = c;
};

unsigned int ir::IR_T_DATABASE_TYPE::count()
{
	if (!_ok) return 0;
	else return _count;
};

unsigned int ir::IR_T_DATABASE_TYPE::tablesize()
{
	if (!_ok) return 0;
	else return _tablesize;
};
