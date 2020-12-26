/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_N2ST_DATABASE
#define IR_N2ST_DATABASE

#include <ir_database/ir_database.h>
#include <ir_mapping.h>
#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <ir_container/ir_block.h>
#include <ir_container/ir_quiet_vector.h>
#include <stdio.h>

namespace ir
{
///@addtogroup ir_database
///@{

	///Number to String Table Database
	///String should be understood as any sequence of bytes
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
			MetaCell() noexcept;
		};

		struct FileMetaCommon
		{
			bool hold				= false;	//defines if program holds file in RAM
			unsigned int pointer	= 0;		//if !hold dublicates ftell, otherwise invalid
			unsigned int size		= 0;		//if hold dublicates ram.size(), otherwise dublicates file size
			FILE *file				= nullptr;	//if !hold is file, otherwize invalid
			bool changed			= false;	//if hold defines if file was changed, otherwise invalid
		};

		struct : FileMetaCommon
		{
			QuietVector<char> ram;				//valid if hold, otherwise nullptr
			unsigned int used				= 0;
		} _file;

		struct : FileMetaCommon
		{
			QuietVector<MetaCell> ram;			//valid if hold, otherwise nullptr
			unsigned int count			= 0;
			unsigned int delcount			= 0;
		} _meta;

		bool _ok					= false;
		bool _writeaccess			= false;
		bool _beta					= false;
		QuietVector<syschar> _path;
		ir::Mapping _mapping;

		//Primitive read & write section
		ec _read(void *buffer, unsigned int offset, unsigned int size)					noexcept;
		ec _write(const void *buffer, unsigned int offset, unsigned int size)			noexcept;
		ec _readpointer(void **p, unsigned int offset, unsigned int size)				noexcept;
		ec _metaread(MetaCell *cell, unsigned int index)								noexcept;
		ec _metawrite(MetaCell cell, unsigned int index)								noexcept;

		//Init section
		ec _check()																		noexcept;
		ec _reopen_write(bool createnew)													noexcept;
		ec _init(const syschar *filepath, create_mode cmode, bool opposite)				noexcept;
		N2STDatabase(const syschar *filepath, create_mode mode, ec *code, bool)			noexcept;
	public:
		
		///Creates a database
		///@param filepath Relative or absolute path to database files
		///@param mode Creation mode
		///@param code Pointer to ir::ec that receives return status if is not nullptr
		N2STDatabase(const syschar *filepath, create_mode mode, ec *code)						noexcept;
		///Returns whether database is ok
		bool ok()																				const noexcept;
		///Ask if identifier exists and can be read if no supernatural error occurs. Is thread-safe if <tt>set_ram_mode(true, rtue) was done</tt>
		///@param index Integer identifier
		ec probe(unsigned int index)															noexcept;
		///Read value related to identifier. Is thread-safe if <tt>set_ram_mode(true, rtue) was done</tt>
		///@param index Integer identifier
		///@param data Pointer to ir::ConstBlock to receive result
		ec read(unsigned int index, ConstBlock *data)											noexcept;
		///Insert value related to identifier into database
		///@param index Integer identifier
		///@param data Related value
		///@param mode Insertion mode
		ec insert(unsigned int index, ConstBlock data, insert_mode mode = insert_mode::always)	noexcept;
		///Delete value from database
		///@param index Integer identifier
		///@param mode Deletion mode
		ec delet(unsigned int index, delete_mode mode = delete_mode::always)					noexcept;
		///Get number of elements stored in database
		unsigned int count()																	const noexcept;
		///Get size of the table, in elements
		unsigned int get_table_size()															const noexcept;
		///Get size of main database (excluding table), in bytes
		unsigned int get_file_size()															const noexcept;
		///Get used size of main database. Database will have this size after optimizing
		unsigned int get_file_used_size()														const noexcept;
		///Sets table size. It may be a good idea to set table size if you know number of elements explicitly
		///@param newtablesize New table size, must be power of two
		ec set_table_size(unsigned int newtablesize)											noexcept;
		///Sets main file size. It may be a good idea to set file size if you know know it explicitly
		///@param newfilesize New file size, in bytes
		ec set_file_size(unsigned int newfilesize)												noexcept;
		///Tells if table and main file need to be kept in RAM or on hard drive. Values from database are read with two database accessions: to table and to main file. So if both are kept in RAM, access costs two RAM accesses. If both are not, access cost two hard drive accesses, etc.
		///@param holdfile Hold main file in RAM
		///@param holdmeta Hold table in RAM
		ec set_ram_mode(bool holdfile, bool holdmeta)											noexcept;
		///Optimize database for size
		ec optimize()																			noexcept;
		///Destroys database and write files kept in RAM to hard drive
		~N2STDatabase()																			noexcept;
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_N2ST_DATABASE_IMPLEMENT)) && !defined(IT_N2ST_DATABASE_NOT_IMPLEMENT)
	#include <implementation/ir_database/ir_n2st_database_implementation.h>
#endif

#endif	//#ifndef IR_N2ST_DATABASE