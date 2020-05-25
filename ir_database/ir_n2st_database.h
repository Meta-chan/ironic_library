/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Ironic Number to String Table Database
//A light and simple database

#ifndef IR_N2ST_DATABASE
#define IR_N2ST_DATABASE

#include <ir_database/ir_database.h>
#include <ir_openmap.h>
#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <ir_container/ir_block.h>
#include <stdio.h>

namespace ir
{
	class N2STDatabase : public Database
	{
	protected:
		
		struct FileHeader
		{
			unsigned char signature[7]	= {'I', 'N', '2', 'S', 'T', 'D', 'F'};
			unsigned char version		= 1;
		};

		struct MetaHeader
		{
			unsigned char signature[7]	= { 'I', 'N', '2', 'S', 'T', 'D', 'M' };
			unsigned char version		= 1;
			unsigned int used			= 0;
			unsigned int count			= 0;
		};

		struct MetaCell
		{
			unsigned int offset;
			unsigned int size : 31;
			unsigned int deleted : 1;
			MetaCell();
		};

		struct FileMetaCommon
		{
			FILE *file				= nullptr;
			bool hold				= false;	//defines if program holds file in RAM
			unsigned int pointer	= 0;		//dublicates ftell
			unsigned int size		= 0;		//dublicates fseek & ftell if !hold, otherwise shows size in RAM
			bool changed			= false;	//defines if file was changed, valid if hold, otherwise false
		};

		struct : FileMetaCommon
		{
			unsigned int used		= 0;
			void *ram				= nullptr;	//valid if hold, otherwise nullptr
		} _file;

		struct : FileMetaCommon
		{
			unsigned int count		= 0;
			unsigned int delcount	= 0;
			MetaCell *ram			= nullptr;	//valid if hold, otherwise nullptr
		} _meta;

		bool _ok					= false;
		bool _writeaccess			= false;
		ir::OpenmapCache _mapcache;

		//Primitive read & write section
		ec _read(void *buffer, unsigned int offset, unsigned int size);
		ec _write(const void *buffer, unsigned int offset, unsigned int size);
		ec _readpointer(void **p, unsigned int offset, unsigned int size);
		ec _metaread(MetaCell *cell, unsigned int index);
		ec _metawrite(MetaCell cell, unsigned int index);

		//Init section
		ec _check(const syschar *filepath, const syschar *metapath); 
		ec _openwrite(const syschar *filepath, const syschar *metapath, bool createnew);
		ec _init(const syschar *filepath, createmode cmode);

	public:
		
		N2STDatabase(const syschar *filepath, createmode createmode, ec *code);
		ec probe(unsigned int index);
		ec read(unsigned int index, ConstBlock *data);
		ec insert(unsigned int index, ConstBlock data, insertmode mode = insert_always);
		ec delet(unsigned int index, deletemode mode = delete_always);
		unsigned int count();
		unsigned int get_table_size();
		unsigned int get_file_size();
		unsigned int get_file_used_size();
		ec set_table_size(unsigned int newtablesize);
		ec set_file_size(unsigned int newfilesize);
		ec set_ram_mode(bool holdfile, bool holdmeta);
		ec optimize();
		~N2STDatabase();
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_N2ST_DATABASE_IMPLEMENT)) && !defined(IT_N2ST_DATABASE_NOT_IMPLEMENT)
	#include <implementation/ir_database/ir_n2st_database_implementation.h>
#endif

#endif	//#ifndef IR_N2ST_DATABASE