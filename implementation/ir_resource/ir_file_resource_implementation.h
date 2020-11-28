/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FILE_RESOURCE_IMPLEMENTATION
#define IR_FILE_RESOURCE_IMPLEMENTATION

FILE *ir::FileIniterFreer::initvalue = nullptr;

void ir::FileIniterFreer::free(FILE *file)
{
	if (file != nullptr) fclose(file);
}
	
#endif	//#ifndef IR_FILE_RESOURCE_IMPLEMENTATION