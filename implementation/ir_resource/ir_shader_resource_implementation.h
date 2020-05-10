/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SHADER_RESOURCE_IMPLEMENTATION
#define IR_SHADER_RESOURCE_IMPLEMENTATION

void ir::HandleResourceFreer::free(GLuint shader)
{
	if (shader != (GLuint)-1) glDeleteShader(shader);
};
	
#endif	//#ifndef IR_SHADER_RESOURCE_IMPLEMENTATION