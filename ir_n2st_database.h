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

#define _CRT_SECURE_NO_WARNINGS
#include "ir_errorcode.h"
#include "ir_syschar.h"
#include "ir_block.h"
#include <stdio.h>

namespace ir
{
	class N2STDatabase
	{
	public:
		
		enum accessmode
		{
			access_new,
			access_readwrite,
			access_read
		};

		enum insertmode
		{
			insert_always,
			insert_existing,
			insert_not_existing
		};

		enum deletemode
		{
			delete_always,
			delete_existing
		};

		enum rammode
		{
			rammode_no,
			rammode_metafile,
			rammode_all
		};

	private:
		
		struct FileHeader
		{
			unsigned char signature[7]	= {'I', 'N', '2', 'S', 'T', 'D', 'F'};
			unsigned char version		= 0;
		};

		struct MetaFileHeader
		{
			unsigned char signature[7]	= { 'I', 'N', '2', 'S', 'T', 'D', 'M' };
			unsigned char version		= 0;
			unsigned int count			= 0;
		};
		
		//rammode _rmode				= rammode::ram_no;
		FILE *_file						= nullptr;
		//void *_ramfile				= nullptr;
		FILE *_metafile					= nullptr;
		//void *_rammetafile			= nullptr;
		bool _ok						= false;
		unsigned int _count				= 0;
		ir::BufferBlock _buffer;	//need to replace with openmap!

	public:
	
		ec _openfile(const syschar *filepath, bool meta, accessmode amode);
		ec _init(const syschar *filepath, accessmode amode);
		N2STDatabase(const syschar *filepath, accessmode amode, rammode rmode, ec *code);
		unsigned int count();
		unsigned int tablesize();
		ec setrammode(rammode rmode);
		ec read(unsigned int number, ConstBlock *data);
		ec insert(unsigned int number, ConstBlock adata, insertmode imode = insert_always);
		ec delet(unsigned int number, deletemode dmode = delete_always);
		~N2STDatabase();
	};
};

#if (defined(IR_N2ST_DATABASE_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include "implementation/ir_n2st_database_implementation.h"
#endif

#endif	//#ifndef IR_N2ST_DATABASE