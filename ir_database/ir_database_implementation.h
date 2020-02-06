/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_DATABASE_IMPLEMENTATION
#define IR_DATABASE_IMPLEMENTATION

ir::Database::Data::Data() : size(0), data(nullptr)
{
};

ir::Database::Data::Data(unsigned int size, const void *data) : size(size), data(data)
{
};

ir::Database::~Database()
{
};

#endif	//#ifndef IR_DATABASE_IMPLEMENTATION