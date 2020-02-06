/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//This is ir::Database, an interface to light ironic databases.
//The names of functions are self-explaining
//The only comment is: ir::Database is a black box that receives some seques of bytes
//and renurns some sequence of bytes
//This sequence is named ir::Database::Data
//It can be some number, some string or some random sequence of bytes 

#ifndef IR_DATABASE
#define IR_DATABASE

#include "ir_errorcode.h"

namespace ir
{
	class Database
	{
	public:
		struct Data
		{
			unsigned int size;
			const void *data;
			Data();
			Data(unsigned int size, const void *data);
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

		virtual unsigned int count() = 0;
		virtual ec read(Data key, Data *data) = 0;
		virtual ec insert(Data key, Data data, insertmode mode = insert_always) = 0;
		virtual ec delet(Data key, deletemode mode = delete_always) = 0;
		virtual ~Database() = 0;
	};
};

#if (defined(IR_DATABASE_IMPLEMENT) || defined(IR_IMPLEMENT))
	#include "ir_database/ir_database_implementation.h"
#endif

#endif	//#ifndef IR_DATABASE