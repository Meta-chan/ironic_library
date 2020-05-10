/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir_utf - simple string codec
//Initialize the library with utf_init(void), then initialize the codecs you use with CODEC.init(void)
//Finalize the codeccs you use with CODEC.free(void) or call utf_free(void)
//See the usage of the functions and the list of the codecs below

#ifndef IR_UTF
#define IR_UTF

//USAGE

//Returns the size of the native char of encoding
typedef unsigned int UTF_charsize(void);

//Converts an Unicode code to specified encoding
//code is Unicode number
//symbols is a pointer to a string. May be nullptr, then nothing is written
//errcode is the default code, it is used when code is not representable in specified encoding
//returns the number of the native symbols required
typedef unsigned int UTF_encode(unsigned int code, void *symbols, unsigned int errcode);

//Converts sprecified encoding to Unicode
//string is a pointer to a string of specified encoding
//nsymbols is a pointer to unsigned int that recieves the number of native symbols were read, may be nullptr
//returns Unicode number
typedef unsigned int UTF_decode(const void *string, unsigned int *nsymbols);

//Frees the codec
typedef void UTF_free(void);

//Initializes the codec, returns true on success
typedef unsigned char UTF_init(void);

struct UTF_Codec
{
	UTF_charsize *charsize;
	UTF_encode *encode;
	UTF_decode *decode;
	UTF_init *init;
	UTF_free *free;
	void *data;
};

//Decodes a null-terminated string1 with codec1 and end encode it to string2 with codec2 (null character is written)
//Replaces the symbols not representable in the destination encoding with errcode
//string2 may be nullptr, then nothing is written
//returns quiantiny of native symbols of encoding 2 reqired (excluding null character)
unsigned int utf_recode(struct UTF_Codec *codec1, const void *string1, unsigned int errcode, struct UTF_Codec *codec2, void *string2);

//Same as above, but returns a new allocated string, which can be freed with free(void) from stdlib.h
void *utf_alloc_recode(struct UTF_Codec *codec1, const void *string1, unsigned int errcode, struct UTF_Codec *codec2);

//Same as above, but returns a pointer to internal buffer. Be carefull, the buffer may intersect with previous returned value
void *utf_buffer_recode(struct UTF_Codec *codec1, const void *string1, unsigned int errcode, struct UTF_Codec *codec2);

//Initializes the library
void utf_init(void);

//Frees the library
void utf_free(void);

//LIST OF CODECS
extern struct UTF_Codec utf_c;		//ACSII
extern struct UTF_Codec utf_utf8;	//UTF8
extern struct UTF_Codec utf_utf16;	//UTF16
extern struct UTF_Codec utf_utf32;	//UTF32
extern struct UTF_Codec utf_1251;	//Windows-1251 (cyrillic)
extern struct UTF_Codec utf_866;	//DOS-866 (cyrillic)

#if (defined(IR_IMPLEMENT) || defined(IR_UTF_IMPLEMENT)) && !defined(IR_UTF_NOT_IMPLEMENT)
	#include <implementation/ir_utf_implementation.h>
#endif

#endif	//#ifndef IR_UTF