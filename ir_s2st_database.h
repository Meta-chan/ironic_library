/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Ironic String to String Meta Database
//A light and simple database

#ifndef IR_S2ST_DATABASE
#define IR_S2ST_DATABASE

#include "ir_errorcode.h"
#include "ir_openmap.h"
#include "ir_syschar.h"
#include "ir_block.h"
#include "ir_database.h"
#include <stdio.h>

namespace ir
{
	class S2STDatabase : public Database
	{
	private:
		
		struct FileHeader
		{
			unsigned char signature[7]	= {'I', 'S', '2', 'S', 'T', 'D', 'F'};
			unsigned char version		= 0;
		};

		struct MetaFileHeader
		{
			unsigned char signature[7]	= { 'I', 'S', '2', 'S', 'T', 'D', 'M' };
			unsigned char version		= 0;
			unsigned int count			= 0;
			unsigned int delcount		= 0;
		};
		
		//File
		unsigned int _filepointer		= 0;		//dublicates ftell
		unsigned int _filesize			= 0;		//dublicated fseek && ftell
		FILE *_file						= nullptr;
		void *_ramfile					= nullptr;
		bool _holdfile					= false;

		//Metafile
		unsigned int _tablesize			= 0;		//dublicates ftell
		unsigned int _metapointer		= 0;		//dublicated fseek && ftell
		FILE *_metafile					= nullptr;
		unsigned int *_rammetafile		= nullptr;
		bool _holdmeta					= false;

		bool _ok						= false;
		bool _writeaccess				= false;
		bool _filechanged				= false;
		bool _metachanged				= false;
		unsigned int _count				= 0;
		unsigned int _delcount			= 0;
		ir::OpenmapCache _mapcache;

		ec _read(void *buffer, unsigned int offset, unsigned int size);
		ec _write(const void *buffer, unsigned int offset, unsigned int size);
		ec _readpointer(void **p, unsigned int offset, unsigned int size);
		ec _metaread(unsigned int *keyoffset, unsigned int index);
		ec _metawrite(unsigned int keyoffset, unsigned int index);

		ec _readsize(unsigned int headeroffset, unsigned int *datasize, unsigned int *headersize);
		ec _writeblock(ConstBlock data, unsigned int *offset);
		
		ec _checkfile(const syschar *filepath);
		ec _checkmetafile(const syschar *filepath);
		ec _openwrite(const syschar *filepath, const syschar *metafilepath, bool createnew);
		ec _init(const syschar *filepath, createmode cmode);
		
		ec _find(ConstBlock key, unsigned int *metaoffset, unsigned int *dataoffset);
		ec _rehash(unsigned int newmetasize);

	public:

		S2STDatabase(const syschar *filepath, createmode mode, ec *code);
		unsigned int count();
		unsigned int tablesize();
		ec setrammode(bool holdfile, bool holdmeta);
		ec directread(unsigned int index, ConstBlock *key, ConstBlock *data);
		ec read(ConstBlock key, ConstBlock *data);
		ec insert(ConstBlock key, ConstBlock data, insertmode mode = insert_always);
		ec delet(ConstBlock key, deletemode mode = delete_always);
		~S2STDatabase();
	};
};

#if (defined(IR_S2ST_DATABASE_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include "implementation/ir_s2st_database_implementation.h"
#endif

#endif	//#ifndef IR_S2ST_DATABASE