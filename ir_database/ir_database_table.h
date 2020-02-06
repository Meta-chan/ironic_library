/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir::TableDatabase, a class implementing ir::Database, a simple ultra-light table database.
//The main principle is: the program reads and writes information about entries into a table of the given size.

#ifndef IR_DATABASE_TABLE
#define IR_DATABASE_TABLE

#include "ir_database/ir_database.h"
#include <Windows.h>
#include "ir_openmap.h"

namespace ir
{
	class TableDatabase : public Database
	{
	public:

		enum tablemode : unsigned char
		{
			number,	//the program lookes into the KEY'th cell of the table and reads a position of the entry
			chain	//the KEY is hashed, the program lookes into the HASH'th cell of the table and reads a position of the entry.
					//if there is a hash match, the program writes one entry after another
		};

		struct Settings
		{
			unsigned int constkeysize	= 0;					//Specify if the key always has a constant size
			unsigned int constdatasize	= 0;					//Specify if the entry always has a constant size
			unsigned int tablesize		= 0;					//Size of the table,
																//should be comparable with the number of entries you store
			unsigned char offsetsize	= sizeof(unsigned int);	//Size of all pointers in the file, 4 is optimal in most cases
			tablemode mode				= tablemode::number;	//Mode. See explanations above
		};

	private:

		struct _FileHeader
		{
			char signature[3]			= { 'I', 'D', 'T' };
			unsigned char version		= 0;
			Settings settings;
			unsigned int count			= 0;
		};

		bool _ok					= false;	//Indicates successfull initialization
		bool _writeaccess			= false;	//Has access rights
		Settings _settings;
		HANDLE _hfile				= NULL;		//Handle of datbase file, not shared
		unsigned int _filesize		= 0;		//Logical size of _hfile, might differ from the real
		wchar_t *_path				= nullptr;	//Path to the database file	
		unsigned int _count			= 0;		//Number of entries

		ir::MapCache _headcache, _bodycache;
		
		void *_openmap(unsigned int offset, unsigned int size, openmapmode mode);
		unsigned int _makemarker(unsigned int size, unsigned char marker[5], bool key);
		ir::ec _readmarker(unsigned int *offset, unsigned int *size, bool key);
		ir::ec _keymatch(unsigned int *offset, const Data *key, bool *match);
		ir::ec _rewriteoffset(unsigned int *offset, unsigned int offsetoffset);
		ir::ec _calculateindex(Data key, unsigned int *index);
		bool _validsettings(const Settings *settings);
		ir::ec _openfile(const Settings *settings);
		ir::ec _createfile(const Settings *settings);

	public:
		virtual unsigned int count();
		unsigned int filesize();
		TableDatabase(const wchar_t* path, const Settings *settings, bool writeaccess, ir::ec *code);
		//path - path to the database file. Might be a valid path to non-existing file
		//settings - settings of the database, see above
		//writeaccess - if false, guarantees that no changes will be made to the file
		//code - a pointer to error code, can be checked after creation
		virtual ~TableDatabase();

		virtual ir::ec read(Data key, Data *data);
		virtual ir::ec insert(Data key, Data data, insertmode mode);
		virtual ir::ec delet(Data data, deletemode mode);
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_DATABASE_TABLE_IMPLEMENT))
	#include "ir_database/ir_database_table_implementation.h"
#endif

#endif	//#ifndef IR_DATABASE_TABLE