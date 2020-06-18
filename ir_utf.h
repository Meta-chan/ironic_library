/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_UTF
#define IR_UTF

///@defgroup ir_utf UTF
///Ironic encoding library, consists of: @n
/// - Codecs - structures of type @c ir_UTF_Codec that store pointers to encoding's methods. See list of aviable codecs in @b Variables. See function descriptions in <b>Typedef Documentation</b> @n
/// - Functions - functions that use two codecs (like @c utf_ercode) or no codecs (like @c ir_utf_init). See function descriptions in <b>Function Documentation</b>
///To use a codec you need to call @c ir_utf_init() and @c your_codec.init()
///@{

///Gets size of native character of the encoding, in bytes
///@return		Size of native character of the encoding, in bytes
typedef unsigned int Ir_utf_charsize(void);

///Converts the Unicode code to the encoding
///@param[in]	code		Unicode code
///@param[out]	symbols		String in specified encoding. May be @c nullptr, then nothing is written
///@param[in]	errcode		Default Unicode code, used when @c code is not representable in specified encoding
///@return					Number of native characters required for @c code
typedef unsigned int Ir_utf_encode(unsigned int code, void *symbols, unsigned int errcode);

///Converts specified encoding to one Unicode code
///@param[in]	string		String in specified encoding
///@param[out]	nsymbols	Pointer to <tt> unsigned int </tt> that receives number of native characters were read. May be @c nullptr
///@return					Unicode code
typedef unsigned int Ir_utf_decode(const void *string, unsigned int *nsymbols);

///Frees the codec
typedef void Ir_utf_free(void);

///Initializes the codec
///@return					1 on success, 0 on fail
typedef unsigned char Ir_utf_init(void);

///Encoding codec, stores function pointers and related data
struct ir_UTF_Codec
{
	Ir_utf_charsize *charsize;
	Ir_utf_encode *encode;
	Ir_utf_decode *decode;
	Ir_utf_init *init;
	Ir_utf_free *free;
	void *data;
};

///Decodes null-terminated source string into null-terminated destination string (null-character is written)
///Replaces the symbols not representable in destination encoding with @c errcode
///@param[in]	codec1	Source codec
///@param[in]	string1	Source string with encoding specified with @c codec1
///@param[in]	errcode	Default Unicode code, which is used when @c code is not representable in destination encoding
///@param[in]	codec2	Destination codec
///@param[out]	string2	Destination string. May be @c nullptr, then nothing is written
///@return				Number of native characters of destination string (excluding null character)
unsigned int ir_utf_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2, void *string2);

///Same as @c ir_utf_recode, but returns a string, allocated with standard C @c malloc
void *ir_utf_alloc_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2);

///Same as @c ir_utf_recode, but returns a pointer to internal buffer. Be careful, there is only one internal buffer!
void *ir_utf_buffer_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2);

///Initializes the codec library
void ir_utf_init(void);

///Frees the codec library
void ir_utf_free(void);

extern struct ir_UTF_Codec ir_utf_c;	///< ACSII codec
extern struct ir_UTF_Codec ir_utf_utf8;	///< UTF8 codec
extern struct ir_UTF_Codec ir_utf_utf16;///< UTF16 codec
extern struct ir_UTF_Codec ir_utf_utf32;///< UTF32 codec
extern struct ir_UTF_Codec ir_utf_1251;	///< Windows-1251 (cyrillic) codec
extern struct ir_UTF_Codec ir_utf_866;	///< DOS-866 (cyrillic) codec

#if (defined(IR_IMPLEMENT) || defined(IR_UTF_IMPLEMENT)) && !defined(IR_UTF_NOT_IMPLEMENT)
	#include <implementation/ir_utf_implementation.h>
#endif

///@}
#endif	//#ifndef IR_UTF