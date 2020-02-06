/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
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

	//common sector
	ec_alloc,
	ec_null,
	ec_create_file,
	ec_open_file,
	ec_path_is_directory,
	ec_path_is_file,
	ec_invalid_signature,
	ec_object_not_ok,

	//Windows specific sector
	ec_windows_register_class,
	ec_windows_create_window,
	ec_windows_getmessage,

	//openmap sector
	ec_openmap,

	//ir_database sector
	ec_database_not_ok,
	ec_database_no_write_access,
	ec_database_not_exists,
	ec_database_already_exists,
	ec_database_overflow,
	ec_database_invalid_settings,
	ec_database_invalid_size,

	//ir_neuro sector
	ec_neuro_invalid_layers,
	ec_neuro_invalid_forward
};

#ifdef __cplusplus
};
#endif

#endif //ifndef IR_ERRORCODE