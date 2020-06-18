/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SHADER_RESOURCE
#define IR_SHADER_RESOURCE

#include <ir_resource/ir_resource.h>

namespace ir
{
///@addtogroup ir_resource
///@{
	
	#ifndef DOXYGEN
		class ShaderIniterFreer
		{
		private:
			static GLuint initvalue;
			static void free(GLuint shader);
			friend Resource<GLuint, ShaderIniterFreer>;
		};
	#endif
	
	///Shader resource
	///Acts almost like OpenGL GLuint
	///If it's value is greater than zero at the end of the visibility range, shader is deleted
	typedef Resource<GLuint, ShaderIniterFreer> ShaderResource;
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_SHADER_RESOURCE_IMPLEMENT)) && !defined(IR_SHADER_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_shader_resource_implementation.h>
#endif

#endif	//#ifndef IR_SHADER_RESOURCE