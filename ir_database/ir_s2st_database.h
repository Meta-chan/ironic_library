/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Ironic String to String Table Database
//A light and simple database

#ifndef IR_S2ST_DATABASE
#define IR_S2ST_DATABASE

#include <ir_database/ir_database.h>
#include <ir_openmap.h>
#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <ir_container/ir_block.h>
#include <stdio.h>

namespace ir
{
	class S2STDatabase : public Database
	{
	private:	

		struct FileHeader
		{
			unsigned char signature[7]	= { 'I', 'S', '2', 'S', 'T', 'D', 'F' };
			unsigned char version		= 1;
		};

		struct MetaHeader
		{
			unsigned char signature[7]	= { 'I', 'S', '2', 'S', 'T', 'D', 'M' };
			unsigned char version		= 1;
			unsigned int count			= 0;
			unsigned int delcount		= 0;
			unsigned int used			= 0;
		};

		struct MetaCell
		{
			unsigned int offset;
			unsigned int keysize : 31;
			unsigned int deleted : 1;
			unsigned int datasize;
			MetaCell();
		};

		struct FileMetaCommon
		{
			bool hold				= false;	//defines if program holds file in RAM
			unsigned int pointer	= 0;		//dublicates ftell
			unsigned int size		= 0;		//dublicates fseek & ftell if !hold, otherwise shows size in RAM
			FILE *file				= nullptr;
			bool changed			= false;	//defines if file was changed, valid if hold, otherwise false
		};

		struct : FileMetaCommon
		{
			void *ram				= nullptr;	//valid if hold, otherwise nullptr
			unsigned int used = 0;
		} _file;

		struct : FileMetaCommon
		{
			MetaCell *ram			= nullptr;	//valid if hold, otherwise nullptr
			unsigned int count		= 0;
			unsigned int delcount	= 0;
		} _meta;

		bool _ok					= false;
		bool _writeaccess			= false;
		ir::OpenmapCache _mapcache;
		
		//Primitive read & write section
		static unsigned int _align(unsigned int i);
		ec _read(void *buffer, unsigned int offset, unsigned int size);
		ec _write(const void *buffer, unsigned int offset, unsigned int size);
		ec _readpointer(void **p, unsigned int offset, unsigned int size);
		ec _metaread(MetaCell *cell, unsigned int index);
		ec _metawrite(MetaCell cell, unsigned int index);

		//Complex section
		unsigned int _hash(ConstBlock key);
		ec _find(ConstBlock key, unsigned int *metaoffset, MetaCell *cell);
		ec _rehash(unsigned int newmetasize);

		//Init section
		ec _check(const syschar *filepath, const syschar *metapath);
		ec _openwrite(const syschar *filepath, const syschar *metapath, bool createnew);
		ec _init(const syschar *filepath, createmode cmode);
	public:

		S2STDatabase(const syschar *filepath, createmode mode, ec *code);
		ec probe(ConstBlock key);
		ec read(ConstBlock key, ConstBlock *data);
		ec read_direct(unsigned int index, ConstBlock *key, ConstBlock *data);
		ec insert(ConstBlock key, ConstBlock data, insertmode mode = insert_always);
		ec delet(ConstBlock key, deletemode mode = delete_always);
		unsigned int count();
		unsigned int get_table_size();
		unsigned int get_file_size();
		unsigned int get_file_used_size();
		ec set_table_size(unsigned int newtablesize);
		ec set_file_size(unsigned int newfilesize);
		ec set_ram_mode(bool holdfile, bool holdmeta);
		ec optimize();

		ec settablesize(unsigned int newtablesize);
		~S2STDatabase();
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_S2ST_DATABASE_IMPLEMENT)) && !defined(IR_S2ST_DATABASE_NOT_IMPLEMENT)
	#include <implementation/ir_database/ir_s2st_database_implementation.h>
#endif

#endif	//#ifndef IR_S2ST_DATABASE