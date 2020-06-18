/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_DATABASE
#define IR_DATABASE

namespace ir
{
///@defgroup ir_database Database
///@{

	///Class containing modes for databases
	class Database
	{
	public:
		///Insertion mode
		enum insertmode
		{
			insert_always,		///< Insert always, replace value if identifier already exists
			insert_existing,	///< Update value only if identifier already exists, otherwise fail
			insert_not_existing	///< Set value only if identifier does not exist, otherwise fail
		};

		///Deletion mode
		enum deletemode
		{
			delete_always,		///< Delete value if identifier exists, do nothing otherwise
			delete_existing		///< Delete value if identifier exist, otherwise fail
		};
		
		///Creation mode
		enum createmode
		{
			create_read,		///< Open database with read access if files are not corrupted
			create_edit,		///< Open database with read and write access if files are not corrupted
			create_new,			///< Create empty database with read and write access, delete existing files
		};
	};

///@}
};

#endif	//#ifndef IR_DATABASE