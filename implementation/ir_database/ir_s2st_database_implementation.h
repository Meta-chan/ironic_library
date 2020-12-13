/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_S2ST_DATABASE_IMPLEMENTATION
#define IR_S2ST_DATABASE_IMPLEMENTATION

#include <ir_resource/ir_memresource.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <share.h>
#endif

ir::S2STDatabase::MetaCell::MetaCell() noexcept
{
	offset = 0;
	keysize = 0;
	datasize = 0;
	deleted = 0;
}

unsigned int ir::S2STDatabase::_align(unsigned int i) noexcept
{
	return (i + sizeof(unsigned int) - 1) & ~(sizeof(unsigned int) - 1);
}

//same as in N2ST
ir::ec ir::S2STDatabase::_read(void *buffer, unsigned int offset, unsigned int size) noexcept
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

//same as in N2ST
ir::ec ir::S2STDatabase::_write(const void *buffer, unsigned int offset, unsigned int size) noexcept
{
	if (_file.hold)
	{
		if (offset + size > _file.size)
		{
			if (!_file.ram.resize(offset + size)) return ec::alloc;
			_file.size = offset + size;
		}
		memcpy(&_file.ram[offset], buffer, size);
		if (size > 0) _file.changed = true;
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

//same as in N2ST
ir::ec ir::S2STDatabase::_readpointer(void **p, unsigned int offset, unsigned int size) noexcept
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
		void *pointer = openmap(&_mapcache, _file.file, offset, size, openmap_mode::read);
		if (pointer == nullptr) return ec::openmap;
		memcpy(p, &pointer, sizeof(void*));
	}
	return ec::ok;
}

//simmilar to N2ST, can be templated
ir::ec ir::S2STDatabase::_metaread(MetaCell *cell, unsigned int index) noexcept
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

ir::ec ir::S2STDatabase::_metawrite(MetaCell cell, unsigned int index) noexcept
{
	if (index >= _meta.size) return ec::read_file;

	if (_meta.hold)
	{
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
		if (fwrite(&cell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::read_file;
		_meta.pointer++;
	}
	return ec::ok;
}

unsigned int ir::S2STDatabase::_hash(ConstBlock key) noexcept
{
	//FNV-1a
	unsigned int h = 2166136261;
	for (unsigned int i = 0; i < key.size; i++)
	{
		unsigned int octet = ((char*)key.data)[i];
		h = h ^ octet;
		h = h * 16777619;
	}
	return h;
}

//index gets an offset in metafile where to write offset
//dataoffset is data offset read from index or 0/1 if not exists/deleted
ir::ec ir::S2STDatabase::_find(ConstBlock key, unsigned int *index, MetaCell *cell) noexcept
{
	unsigned int searchindex = _hash(key) & (_meta.size - 1);

	//Search for free space in metafile
	while (true)
	{
		//Reading file offset in metafile. If eof go to begin
		MetaCell searchcell;
		ec code = _metaread(&searchcell, searchindex);
		if (code != ec::ok) return code;

		//Check if not exists, deleted or exists
		if (searchcell.offset == 0)
		{
			*index = searchindex;
			*cell = searchcell;
			break;
		}
		else if (searchcell.deleted > 0)
		{
			*index = searchindex;
			*cell = searchcell;
		}
		else
		{
			if (searchcell.keysize == key.size)
			{
				void *readkey = nullptr;
				code = _readpointer(&readkey, searchcell.offset, key.size);
				if (code != ec::ok) return code;
				if (memcmp(key.data, readkey, key.size) == 0)
				{
					*index = searchindex;
					*cell = searchcell;
					break;
				}
			}
		}

		searchindex++;
		if (searchindex == _meta.size) searchindex = 0;
	}
	return ec::ok;
}

//simmilar to N2ST, can be templated
ir::ec ir::S2STDatabase::_rehash(unsigned int newtablesize) noexcept
{
	QuietVector<MetaCell> new_meta;
	if (!new_meta.resize(newtablesize)) return ec::alloc;

	//Rehashing
	for (unsigned int i = 0; i < _meta.size; i++)
	{
		//Reading meta
		MetaCell cell;
		ec code = _metaread(&cell, i);
		if (code != ec::ok) return code;

		if (cell.offset != 0 && cell.deleted == 0)
		{
			//Reading key
			void *readkey = nullptr;
			code = _readpointer(&readkey, cell.offset, cell.keysize);
			if (code != ec::ok) return code;

			unsigned int searchindex = _hash(ConstBlock(cell.keysize, readkey)) & (newtablesize - 1);

			//Inserting meta to new table
			while (true)
			{
				if (new_meta[searchindex].offset == 0) { new_meta[searchindex] = cell; break; }
				else { searchindex++; if (searchindex == newtablesize) searchindex = 0; }
			}
		}
	}

	//Swapping
	if (_meta.hold)
	{
		_meta.ram.assign(new_meta);
	}
	else
	{
		if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::seek_file;
		if (fwrite(&new_meta[0], sizeof(MetaCell), newtablesize, _meta.file) < newtablesize) return ec::write_file;
		_meta.pointer = newtablesize;
	}
	_meta.size = newtablesize;
	_meta.delcount = 0;
	return ec::ok;
}

//File part is simmilar as in N2ST
ir::ec ir::S2STDatabase::_check(const syschar *filepath, const syschar *metapath) noexcept
{
	//FILE
	#ifdef _WIN32
		_file.file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		_file.file = fopen(filepath, "rb");
	#endif

	if (_file.file == nullptr) return ec::open_file;
	FileHeader header, sample;
	if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::seek_file;
	if (fread(&header, sizeof(FileHeader), 1, _file.file) == 0
	|| memcmp(&header, &sample, sizeof(FileHeader)) != 0) return ec::invalid_signature;

	if (fseek(_file.file, 0, SEEK_END) != 0) return ec::seek_file;
	_file.size = ftell(_file.file);
	_file.pointer = _file.size;

	//META
	#ifdef _WIN32
		_meta.file = _wfsopen(metapath, L"rb", _SH_DENYNO);
	#else
		_meta.file = fopen(metapath, "rb");
	#endif

	if (_meta.file == nullptr) return ec::open_file;
	MetaHeader metaheader, metasample;
	if (fseek(_meta.file, 0, SEEK_SET) != 0) return ec::seek_file;
	if (fread(&metaheader, sizeof(MetaHeader), 1, _meta.file) == 0
	|| memcmp(metaheader.signature, metasample.signature, 7) != 0
	|| metaheader.version != metaheader.version) return ec::invalid_signature;

	_file.used = metaheader.used;
	if (_file.used > _file.size) return ec::invalid_signature;

	if (fseek(_meta.file, 0, SEEK_END) != 0) return ec::seek_file;
	_meta.size = ftell(_meta.file) - sizeof(MetaHeader);
	if (_meta.size < sizeof(MetaCell)) return ec::invalid_signature;
	if ((_meta.size % sizeof(MetaCell)) != 0) return ec::invalid_signature;
	_meta.size /= sizeof(MetaCell);
	if ((_meta.size & (_meta.size - 1)) != 0) return ec::invalid_signature;
	_meta.pointer = _meta.size;

	_meta.count = metaheader.count;
	_meta.delcount = metaheader.delcount;
	if (_meta.count + _meta.delcount> _file.size) return ec::invalid_signature;

	return ec::ok;
}

//File part is simmilar as in N2ST
ir::ec ir::S2STDatabase::_openwrite(const syschar *filepath, const syschar *metapath, bool createnew) noexcept
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
	if (_file.file == nullptr) return ec::create_file;
	if (createnew)
	{
		FileHeader header;
		if (fwrite(&header, sizeof(FileHeader), 1, _file.file) == 0) return ec::write_file;
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
	if (_meta.file == nullptr) return ec::create_file;
	if (createnew)
	{
		MetaHeader header;
		if (fwrite(&header, sizeof(MetaHeader), 1, _meta.file) == 0) return ec::write_file;
		MetaCell nullcell;
		if (fwrite(&nullcell, sizeof(MetaCell), 1, _meta.file) == 0) return ec::write_file;
		_meta.pointer = 1;
		_meta.size = 1;
	}

	_writeaccess = true;
	return ec::ok;
}

//same as in N2ST
ir::ec ir::S2STDatabase::_init(const syschar *filepath, create_mode mode) noexcept
{
	#ifdef _WIN32
		size_t pathlen = wcslen(filepath);
	#else
		size_t pathlen = strlen(filepath);
	#endif

	MemResource<syschar> metafilepath = (syschar*)malloc((pathlen + 2) * sizeof(syschar));
	if (metafilepath == nullptr) return ec::alloc;
	memcpy(metafilepath, filepath, pathlen * sizeof(syschar));
	metafilepath[pathlen] = '~';
	metafilepath[pathlen + 1] = '\0';

	if (mode == create_mode::neww)
	{
		ec code = _openwrite(filepath, metafilepath, true);
		if (code != ec::ok) return code;
	}
	else
	{
		ec code = _check(filepath, metafilepath);
		if (code != ec::ok) return code;
		if (mode == create_mode::edit)
		{
			code = _openwrite(filepath, metafilepath, false);
			if (code != ec::ok) return code;
		}
	}

	_ok = true;
	return ec::ok;
}


//Same as in N2ST
ir::S2STDatabase::S2STDatabase(const syschar *filepath, create_mode createmode, ec *code) noexcept
{
	ec c = _init(filepath, createmode);
	if (code != nullptr) *code = c;
}

bool ir::S2STDatabase::ok() const noexcept
{
	return _ok;
}

ir::ec ir::S2STDatabase::probe(ConstBlock key) noexcept
{
	if (!_ok) return ec::object_not_inited;
	
	//Find key
	unsigned int index = 0;
	MetaCell cell;
	ec code = _find(key, &index, &cell);
	if (code != ec::ok) return code;
	if (cell.offset == 0 || cell.deleted > 0) return ec::key_not_exists;

	return ec::ok;
}

ir::ec ir::S2STDatabase::read(ConstBlock key, ConstBlock *data) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (data == nullptr) return ec::null;

	//Find key
	unsigned int index = 0;
	MetaCell cell;
	ec code = _find(key, &index, &cell);
	if (code != ec::ok) return code;
	if (cell.offset == 0 || cell.deleted > 0) return ec::key_not_exists;

	//Read data
	void *readdata = nullptr;
	unsigned int alignoffset = _align(cell.offset + cell.keysize);
	code = _readpointer(&readdata, alignoffset, cell.datasize);
	data->data = readdata;
	data->size = cell.datasize;

	return ec::ok;
}

ir::ec ir::S2STDatabase::read_direct(unsigned int index, ConstBlock *key, ConstBlock *data) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (key == nullptr && data == nullptr) return ec::null;
	if (index >= _meta.size) return ec::key_not_exists;

	//Find dataoffset
	MetaCell cell;
	ec code = _metaread(&cell, index);
	if (code != ec::ok) return code;
	if (cell.offset == 0 || cell.deleted > 0) return ec::key_not_exists;

	if (key != nullptr && data == nullptr)
	{
		//Read only key
		void *readkey = nullptr;
		code = _readpointer(&readkey, cell.offset, cell.keysize);
		if (code != ec::ok) return code;
		key->data = readkey;
		key->size = cell.keysize;
		return ec::ok;
	}
	else if (key == nullptr && data != nullptr)
	{
		//Read only data
		void *readdata = nullptr;
		unsigned int alignoffset = _align(cell.offset + cell.keysize);
		code = _readpointer(&readdata, alignoffset, cell.datasize);
		if (code != ec::ok) return code;
		data->data = readdata;
		data->size = cell.datasize;
	}
	else
	{
		void *readkeydata = nullptr;
		code = _readpointer(&readkeydata, cell.offset, _align(cell.keysize) + cell.datasize);
		if (code != ec::ok) return code;
		key->data = readkeydata;
		key->size = cell.keysize;
		data->data = (char*)readkeydata + _align(cell.keysize);
		data->size = cell.datasize;
	}

	return ec::ok;
}

ir::ec ir::S2STDatabase::insert(ConstBlock key, ConstBlock data, insert_mode mode) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;

	//Find cell
	MetaCell cell;
	unsigned int index = 0;
	ec code = _find(key, &index, &cell);
	if (code != ec::ok) return code;
	
	bool found = cell.offset != 0 && cell.deleted == 0;
	bool deleted = cell.offset != 0 && cell.deleted > 0;
	if (mode == insert_mode::existing && !found) return ec::key_not_exists;
	else if (mode == insert_mode::not_existing && found) return ec::key_already_exists;
	
	//If exists or deleted and size is sufficient
	unsigned int oldsize = cell.datasize;
	if (cell.offset != 0 && cell.datasize >= data.size)
	{
		//If something need to be changed
		if (cell.datasize != data.size || cell.deleted > 0)
		{
			cell.datasize = data.size;
			cell.deleted = 0;
			code = _metawrite(cell, index);
			if (code != ec::ok) return code;
		}
	}
	else
	{
		cell.datasize = data.size;
		cell.keysize = key.size;
		cell.deleted = 0;
		cell.offset = _align(_file.size);
		code = _metawrite(cell, index);
		if (code != ec::ok) return code;
		code = _write(key.data, cell.offset, key.size);
		if (code != ec::ok) return code;
	}
	code = _write(data.data, _align(cell.offset + cell.keysize), data.size);
	if (code != ec::ok) return code;

	if (found)
	{
		_file.used = _file.used + data.size - oldsize;
	}
	else
	{
		if (deleted) _meta.delcount--;
		_file.used += (data.size + key.size);
		_meta.count++;
	}
	if (2 * (_meta.count + _meta.delcount) > _meta.size) return _rehash(2 * _meta.size);
	return ec::ok;
}

ir::ec ir::S2STDatabase::delet(ConstBlock key, delete_mode mode) noexcept
{
	if (!_ok) return ec::object_not_inited;
	if (!_writeaccess) return ec::write_file;

	//Find cell
	MetaCell cell;
	unsigned int index = 0;
	ec code = _find(key, &index, &cell);
	if (code != ec::ok) return code;
	bool found = cell.offset != 0 && cell.deleted == 0;
	
	if (!found)
	{
		if (mode == delete_mode::existing) return ec::key_not_exists;
		else return ec::ok;
	}

	cell.deleted = 1;
	code = _metawrite(cell, index);
	_meta.delcount++;

	return ec::ok;
}

//Same as in N2ST
unsigned int ir::S2STDatabase::count() const noexcept
{
	if (!_ok) return 0;
	else return _meta.count;
}

//Same as in N2ST
unsigned int ir::S2STDatabase::get_table_size() const noexcept
{
	if (!_ok) return 0;
	else return _meta.size;
}

//Same as in N2ST
unsigned int ir::S2STDatabase::get_file_size() const noexcept
{
	if (!_ok) return 0;
	else return _file.size;
}

//Same as in N2ST
unsigned int ir::S2STDatabase::get_file_used_size() const noexcept
{
	if (!_ok) return 0;
	else return _file.used;
}

ir::ec ir::S2STDatabase::set_table_size(unsigned int newtablesize) noexcept
{
	if (!_ok) return ec::object_not_inited;
	else return ec::not_implemented;
}

ir::ec ir::S2STDatabase::set_file_size(unsigned int newfilesize) noexcept
{
	if (!_ok) return ec::object_not_inited;
	else return ec::not_implemented;
}

//Simmilar to N2ST, can be templated
ir::ec ir::S2STDatabase::set_ram_mode(bool holdfile, bool holdmeta) noexcept
{
	if (!_ok) return ec::object_not_inited;

	//Read meta
	if (holdmeta && !_meta.hold)
	{
		if (!_meta.ram.resize(_meta.size)) return ec::alloc;
		if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::seek_file;;
		if (fread(&_meta.ram[0], sizeof(MetaCell), _meta.size, _meta.file) < _meta.size) return ec::read_file;
		_meta.pointer = _meta.size;
	}
	//Write meta
	else if (!holdmeta && _meta.hold)
	{
		if (_writeaccess && _meta.changed)
		{
			if (fseek(_meta.file, sizeof(MetaHeader), SEEK_SET) != 0) return ec::seek_file;
			if (fwrite(&_meta.ram[0], sizeof(MetaCell), _meta.size, _meta.file) < _meta.size) return ec::write_file;
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
		if (fread(&_file.ram[0], 1, _file.size, _file.file) < _file.size) return ec::read_file;
		_file.pointer = _file.size;
	}
	//Write data
	else if (!holdfile && _file.hold)
	{
		if (_writeaccess && _file.changed)
		{
			if (fseek(_file.file, 0, SEEK_SET) != 0) return ec::seek_file;;
			if (fwrite(&_file.ram[0], 1, _file.size, _file.file) < _file.size) return ec::write_file;
			_file.pointer = _file.size;
		}
		_file.ram.clear();
	}
	_file.hold = holdfile;
	_file.changed = false;

	return ec::ok;
}

ir::S2STDatabase::~S2STDatabase() noexcept
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
			header.delcount = _meta.delcount;
			header.used = _file.used;
			fseek(_meta.file, 0, SEEK_SET);
			fwrite(&header, sizeof(MetaHeader), 1, _meta.file);
			
		}
		fclose(_meta.file);
	}
}

#endif	//#ifndef IR_S2ST_DATABASE_IMPLEMENTATION