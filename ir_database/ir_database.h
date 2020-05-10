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
	class Database
	{
	public:
	
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

		enum createmode
		{
			create_readonly,
			create_new_never,
			create_new_if_no,
			create_new_if_corrupted,
			create_new_if_no_or_corrupted,
			create_new_always
		};
	};
};

#endif	//#ifndef IR_DATABASE