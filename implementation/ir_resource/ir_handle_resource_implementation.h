/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_HANDLE_RESOURCE_IMPLEMENTATION
#define IR_HANDLE_RESOURCE_IMPLEMENTATION

void ir::HandleResourceFreer::free(HANDLE handle)
{
	if (handle != NULL && handle != INVALID_HANDLE_VALUE) CloseHandle(handle);
};
	
#endif	//#ifndef IR_HANDLE_RESOURCE_IMPLEMENTATION