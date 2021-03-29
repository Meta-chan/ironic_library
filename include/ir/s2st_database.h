/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_S2ST_DATABASE
#define IR_S2ST_DATABASE

#include "database.h"
#include "block.h"
#include "mapping.h"
#include "ec.h"
#include "types.h"
#include "quiet_vector.h"
#include <stdio.h>

namespace ir
{
///@addtogroup database Databases
///@{

	///String to String Table Database
	///String should be understood as any sequence of bytes
	class S2STDatabase : public Database
	{
	protected:	

		struct FileHeader
		{
			unsigned char signature[7]	= { 'I', 'S', '2', 'S', 'T', 'D', 'F' };
			unsigned char version		= 1;
		};

		struct MetaHeader
		{
			unsigned char signature[7]	= { 'I', 'S', '2', 'S', 'T', 'D', 'M' };
			unsigned char version		= 1;
			uint32 count				= 0;
			uint32 delcount				= 0;
			uint32 used					= 0;
		};

		struct MetaCell
		{
			uint32 offset;
			uint32 keysize : 31;
			uint32 deleted : 1;
			uint32 datasize;
			MetaCell() noexcept;
		};

		struct FileMetaCommon
		{
			bool hold		= false;	//defines if program holds file in RAM
			uint32 pointer	= 0;		//if !hold duplicates ftell, otherwise invalid
			uint32 size		= 0;		//if hold duplicates ram.size(), otherwise duplicates file size
			FILE *file		= nullptr;	//if !hold is file, otherwise invalid
			bool changed	= false;	//if hold defines if file was changed, otherwise invalid
		};

		struct : FileMetaCommon
		{
			QuietVector<char> ram;		//valid if hold, otherwise empty
			uint32 used		= 0;
		} _file;

		struct : FileMetaCommon
		{
			QuietVector<MetaCell> ram;	//valid if hold, otherwise empty
			uint32 count	= 0;
			uint32 delcount	= 0;
		} _meta;

		QuietVector<schar> _path;
		bool _beta			= false;
		bool _ok			= false;
		bool _writeaccess	= false;
		ir::Mapping _mapping;
		
		//Primitive read & write section
		static uint32 _align(uint32 i)											noexcept;
		ec _read(void *buffer, uint32 offset, uint32 size)						noexcept;
		ec _write(const void *buffer, uint32 offset, uint32 size)				noexcept;
		ec _readpointer(void **p, uint32 offset, uint32 size)					noexcept;
		ec _metaread(MetaCell *cell, uint32 index)								noexcept;
		ec _metawrite(MetaCell cell, uint32 index)								noexcept;

		//Complex section
		ec _find(Block key, uint32 *metaoffset, MetaCell *cell)					noexcept;
		ec _rehash(uint32 newmetasize)											noexcept;

		//Init section
		ec _check()																noexcept;
		ec _reopen_write(bool createnew)										noexcept;
		ec _init(const schar *filepath, create_mode mode, bool opposite)		noexcept;
		S2STDatabase(const schar *filepath, create_mode mode, ec *code, bool)	noexcept;
	public:
		///Creates empty database
		S2STDatabase()															noexcept;
		///Creates database
		///@param filepath Relative or absolute path to database files
		///@param mode Creation mode
		///@param code Pointer to ir::ec that receives return status if is not `nullptr`
		S2STDatabase(const schar *filepath, create_mode mode, ec *code)			noexcept;
		///Initializes database
		///@param filepath Relative or absolute path to database files
		///@param mode Creation mode
		ec init(const schar *filepath, create_mode mode)						noexcept;
		///Returns whether database is ok
		bool ok()																const noexcept;
		///Asks if identifier exists and can be read if no supernatural error occurs. Is thread-safe if `set_ram_mode(true, true)` was done
		///@param key String identifier
		ec probe(Block key)														noexcept;
		///Reads value related to identifier. Is thread-safe if `set_ram_mode(true, true)` was done
		///@param key String identifier
		///@param data Pointer to ir::Block to receive result
		ec read(Block key, Block *data)											noexcept;
		///Reads value of given index from table. May be used to search in database
		///@param index Index in table
		///@param key Pointer to ir::Block to receive identifier, may be `nullptr`
		///@param data Pointer to ir::Block to receive value, may be `nullptr`
		ec read_direct(uint32 index, Block *key, Block *data)					noexcept;
		///Inserts value related to identifier into database
		///@param key String identifier
		///@param data Related value
		///@param mode Insertion mode
		ec insert(Block key, Block data, insert_mode mode = insert_mode::always)noexcept;
		///Delete value from database
		///@param key String identifier
		///@param mode Deletion mode
		ec delet(Block key, delete_mode mode = delete_mode::always)				noexcept;
		///Gets number of elements stored in database
		uint32 count()															const noexcept;
		///Gets size of the table, in elements
		uint32 get_table_size()													const noexcept;
		///Gets size of main database (excluding table), in bytes
		uint32 get_file_size()													const noexcept;
		///Gets used size of main database. Database will have this size after optimizing
		uint32 get_file_used_size()												const noexcept;
		///Sets table size. It may be a good idea to set table size if you know number of elements explicitly
		///@param newtablesize New table size, must be power of two
		ec set_table_size(uint32 newtablesize)									noexcept;
		///Sets main file size. It may be a good idea to set file size if you know know it explicitly
		///@param newfilesize New file size, in bytes
		ec set_file_size(uint32 newfilesize)									noexcept;
		///Tells if table and main file need to be kept in RAM or on hard drive. Values from database are read with two database accessions: to table and to main file. So if both are kept in RAM, access costs two RAM accesses. If both are not, access costs two hard drive accesses, etc.
		///@param holdfile Hold main file in RAM
		///@param holdmeta Hold table in RAM
		ec set_ram_mode(bool holdfile, bool holdmeta)							noexcept;
		///Optimizes database for size
		ec optimize()															noexcept;
		///Finalized database and frees resources
		void finalize()															noexcept;
		///Destroys database and writes files kept in RAM to hard drive
		~S2STDatabase()															noexcept;
	};
	
///@}	
}

#endif	//#ifndef IR_S2ST_DATABASE

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_S2ST_DATABASE) : !defined(IR_EXCLUDE_S2ST_DATABASE)
	#ifndef IR_INCLUDE

	#elif IR_INCLUDE == 'a'
		#ifndef IR_S2ST_DATABASE_SOURCE
			#define IR_S2ST_DATABASE_SOURCE
			#include "../../source/s2st_database.h"
		#endif
	#endif
#endif