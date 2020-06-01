/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Standart error code of the Ironic Project

#ifndef IR_ERRORCODE
#define IR_ERRORCODE

#ifdef __cplusplus
namespace ir
{
	enum ec : unsigned int
#else
	enum ir_ec : unsigned int
#endif

	{
		ec_ok = 0,
		ec_other = 1,
		ec_not_implemented = 2,

		//common sector
		ec_alloc,
		ec_null,
		ec_create_file,
		ec_open_file,
		ec_read_file,
		ec_write_file,
		ec_seek_file,
		ec_path_is_directory,
		ec_path_is_file,
		ec_invalid_signature,
		ec_object_not_ok,
		ec_align,
		ec_invalid_input,

		//Windows specific sector
		ec_windows_register_class,
		ec_windows_create_window,
		ec_windows_getmessage,
		ec_windows_createthread,

		//openmap sector
		ec_openmap,

		//databases and registers sector
		ec_key_not_exists,
		ec_key_already_exists,

		//opengl sector
		ec_opengl
	};

#ifdef __cplusplus
};
#endif

#endif //ifndef IR_ERRORCODE