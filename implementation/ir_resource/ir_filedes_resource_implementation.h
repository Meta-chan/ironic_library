/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FILEDES_RESOURCE_IMPLEMENTATION
#define IR_FILEDES_RESOURCE_IMPLEMENTATION

int ir::FiledesIniterFreer::initvalue = -1;

void ir::FiledesIniterFreer::free(int filedes)
{
	if (filedes >= 0) close(filedes);
};
	
#endif	//#ifndef IR_FILEDES_RESOURCE_IMPLEMENTATION