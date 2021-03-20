/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_DATABASE
#define IR_DATABASE

namespace ir
{
///@addtogroup database Databases
///@{

	///Class containing modes for databases
	class Database
	{
	public:
		///Insertion mode
		enum class insert_mode
		{
			always,			///< Insert always, replace value if identifier already exists
			existing,		///< Update value only if identifier already exists, otherwise fail
			not_existing	///< Set value only if identifier does not exist, otherwise fail
		};

		///Deletion mode
		enum class delete_mode
		{
			always,		///< Delete value if identifier exists, do nothing otherwise
			existing	///< Delete value if identifier exist, otherwise fail
		};
		
		///Creation mode
		enum create_mode
		{
			read,		///< Open database with read access if files are not corrupted
			edit,		///< Open database with read and write access if files are not corrupted
			neww		///< Create empty database with read and write access, delete existing files
		};
	};

///@}
}

#endif	//#ifndef IR_DATABASE