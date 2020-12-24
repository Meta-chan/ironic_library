/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_UTF_IMPLEMENTATION
#define IR_UTF_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

struct ir_UTF_Codec ir_utf_c;
struct ir_UTF_Codec ir_utf_utf8;
struct ir_UTF_Codec ir_utf_utf16;
struct ir_UTF_Codec ir_utf_utf32;
struct ir_UTF_Codec ir_utf_1251;
struct ir_UTF_Codec ir_utf_866;

struct
{
	void *data;
	unsigned int reserved;
} _ir_internal_utf_buffer;

#ifdef _WIN32
	#pragma region ASCII
#endif

unsigned int _ir_internal_utf_c_charsize(void)
{
	return 1;
}

unsigned int _ir_internal_utf_c_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	unsigned char *s = (unsigned char*)symbols;
	if (code < 0x80)
	{
		if (s != NULL) s[0] = (unsigned char)code;
		return 1;
	}
	else return _ir_internal_utf_c_encode(errcode, symbols, ' ');
}

unsigned int _ir_internal_utf_c_decode(const void *string, unsigned int *nsymbols)
{
	unsigned char c = *((unsigned char*)string);
	if (c < 0x80)
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return c;
	}
	else
	{
		*nsymbols = 0;
		return 0;
	}
}

void _ir_internal_utf_c_free(void)
{
}

unsigned char _ir_internal_utf_c_init(void)
{
	ir_utf_c.charsize = _ir_internal_utf_c_charsize;
	ir_utf_c.encode = _ir_internal_utf_c_encode;
	ir_utf_c.decode = _ir_internal_utf_c_decode;
	ir_utf_c.free = _ir_internal_utf_c_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF8
#endif

unsigned int _ir_internal_utf_utf8_charsize(void)
{
	return 1;
}

unsigned int _ir_internal_utf_utf8_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	unsigned char *s = (unsigned char*)symbols;
	if (code < 0x80)
	{
		if (s != NULL) s[0] = (unsigned char) code;
		return 1;
	}
	else if (code < 0x800)
	{
		if (s != NULL)
		{
			s[0] = 0xC0 | ((code >> 6) & 0x1F);
			s[1] = 0x80 | (code & 0x3F);
		}
		return 2;
	}
	else if (code < 0x10000)
	{
		if (s != NULL)
		{
			s[0] = 0xE0 | ((code >> 12) & 0x0F);
			s[1] = 0x80 | ((code >> 6) & 0x3F);
			s[2] = 0x80 | (code & 0x3F);
		}
		return 3;
	}
	else if (code < 0x110000)
	{
		if (s != NULL)
		{
			s[0] = 0xE0 | ((code >> 18) & 0x07);
			s[1] = 0x80 | ((code >> 12) & 0x3F);
			s[2] = 0x80 | ((code >> 6) & 0x3F);
			s[3] = 0x80 | (code & 0x3F);
		}
		return 4;
	}
	else return _ir_internal_utf_utf8_encode(errcode, symbols, ' ');
}

unsigned int _ir_internal_utf_utf8_decode(const void *string, unsigned int *nsymbols)
{
	const unsigned char *s = (const unsigned char*)string;
	if ((s[0] & 0x80) == 0)
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return s[0];
	}
	else if ((s[0] & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80)
	{
		if (nsymbols != NULL) *nsymbols = 2;
		return ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
	}
	else if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (nsymbols != NULL) *nsymbols = 3;
		return ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
	}
	else if ((s[0] & 0xF8) == 0xF0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80)
	{
		if (nsymbols != NULL) *nsymbols = 4;
		return ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
	}
	else
	{
		*nsymbols = 0;
		return 0;
	}
}

void _ir_internal_utf_utf8_free(void)
{
}

unsigned char _ir_internal_utf_utf8_init(void)
{
	ir_utf_utf8.charsize = _ir_internal_utf_utf8_charsize;
	ir_utf_utf8.encode = _ir_internal_utf_utf8_encode;
	ir_utf_utf8.decode = _ir_internal_utf_utf8_decode;
	ir_utf_utf8.free = _ir_internal_utf_utf8_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF16
#endif

unsigned int _ir_internal_utf_utf16_charsize(void)
{
	return 2;
}

unsigned int _ir_internal_utf_utf16_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	unsigned short int *s = (unsigned short int*)symbols;
	if (code < 0x10000 && !(code >= 0xD800 && code < 0xE000))
	{
		if (s != NULL) s[0] = (unsigned short)code;
		return 1;
	}
	else if (code < 0x110000)
	{
		if (s != NULL)
		{
			s[0] = ((code - 0x10000) >> 10) & 0x3FF;
			s[1] = (code - 0x10000) & 0x3FF;
		}
		return 2;
	}
	else return _ir_internal_utf_c_encode(errcode, symbols, ' ');
}

unsigned int _ir_internal_utf_utf16_decode(const void *string, unsigned int *nsymbols)
{
	const unsigned short int *s = (const unsigned short int*)string;
	if (!(s[0] >= 0xD800 && s[0] < 0xE000))
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return s[0];
	}
	else if ((s[0] >= 0xD800 && s[0] < 0xDC00) && (s[1] >= 0xDC00 && s[1] < 0xE000))
	{
		if (nsymbols != NULL) *nsymbols = 2;
		return 0;
	}
	else
	{
		*nsymbols = 0;
		return 0;
	}
}

void _ir_internal_utf_utf16_free(void)
{
}

unsigned char _ir_internal_utf_utf16_init(void)
{
	ir_utf_utf16.charsize = _ir_internal_utf_utf16_charsize;
	ir_utf_utf16.encode = _ir_internal_utf_utf16_encode;
	ir_utf_utf16.decode = _ir_internal_utf_utf16_decode;
	ir_utf_utf16.free = _ir_internal_utf_utf16_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF32
#endif

unsigned int _ir_internal_utf_utf32_charsize(void)
{
	return 4;
}

unsigned int _ir_internal_utf_utf32_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	if (symbols != NULL) *((unsigned int*)symbols) = code;
	return 1;
}

unsigned int _ir_internal_utf_utf32_decode(const void *string, unsigned int *nsymbols)
{
	if (nsymbols != NULL) *nsymbols = 1;
	return *((unsigned int*) nsymbols);
}

void _ir_internal_utf_utf32_free(void)
{
}

unsigned char _ir_internal_utf_utf32_init(void)
{
	ir_utf_utf32.charsize = _ir_internal_utf_utf32_charsize;
	ir_utf_utf32.encode = _ir_internal_utf_utf32_encode;
	ir_utf_utf32.decode = _ir_internal_utf_utf32_decode;
	ir_utf_utf32.free = _ir_internal_utf_utf32_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
	#pragma region 1251
#endif

const unsigned short int _ir_internal_utf_1251_symbols[0x40] =
{
	0x402,	0x403,	0x201A,	0x453,	0x201E,	0x2026,	0x2020,	0x2021,	0x20AC,	0x2030,	0x409,	0x2039,	0x40A,	0x40C,	0x40B,	0x40F,
	0x452,	0x2018,	0x2019,	0x201C,	0x201D,	0x2022,	0x2013,	0x2014,	0x0,	0x2122,	0x459,	0x203A,	0x45A,	0x45C,	0x45B,	0x45F,
	0xA0,	0x40E,	0x45E,	0x408,	0xA4,	0x490,	0xA6,	0xA7,	0x401,	0xA9,	0x404,	0xAB,	0xAC,	0xAD,	0xAE,	0x407,
	0xB0,	0xB1,	0x406,	0x456,	0x491,	0xB5,	0xB6,	0xB7,	0x451,	0x2116,	0x454,	0xBB,	0x458,	0x405,	0x455,	0x457
};

unsigned int _ir_internal_utf_1251_charsize(void)
{
	return 1;
}

unsigned int _ir_internal_utf_1251_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	unsigned char *s = (unsigned char*)symbols;
	
	if (code < 0x80)
	{
		if (s != NULL) s[0] = (unsigned char)code;
		return 1;
	}
	else if (code >= 0x410 && code < 0x450)
	{
		if (s != NULL) s[0] = (unsigned char)(code - 0x410 + 0xC0);
		return 1;
	}
	else if (code >= 0xA0 && code <= 0x2122)
	{
		for (unsigned int i = 0; i < 0x40; i++)
		{
			//can be optimized with binar search
			if (_ir_internal_utf_1251_symbols[i] == code)
			{
				if (s != NULL) s[0] = (unsigned char)(0x80 + i);
				return 1;
			}
		}
		return _ir_internal_utf_1251_encode(errcode, symbols, ' ');
	}
	else return _ir_internal_utf_1251_encode(errcode, symbols, ' ');
}

unsigned int _ir_internal_utf_1251_decode(const void *string, unsigned int *nsymbols)
{
	unsigned char c = *((unsigned char*)string);
	if (c < 0x80)
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return c;
	}
	else if (c < 0xC0)
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return _ir_internal_utf_1251_symbols[c - 0x80];
	}
	else if (c != 0x98)
	{
		if (nsymbols != NULL) *nsymbols = 1;
		return c - 0xC0 + 0x410;
	}
	else
	{
		*nsymbols = 0;
		return 0;
	}
}

void _ir_internal_utf_1251_free(void)
{
}

unsigned char _ir_internal_utf_1251_init(void)
{
	ir_utf_1251.charsize = _ir_internal_utf_1251_charsize;
	ir_utf_1251.encode = _ir_internal_utf_1251_encode;
	ir_utf_1251.decode = _ir_internal_utf_1251_decode;
	ir_utf_1251.free = _ir_internal_utf_1251_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
	#pragma region 866
#endif

const unsigned short int _ir_internal_utf_866_symbols1[0x30] =
{
	0x2591,	0x2592,	0x2593,	0x2502,	0x2524,	0x2561,	0x2562,	0x2556,	0x2555,	0x2563,	0x2551,	0x2557,	0x255D,	0x255C,	0x255B,	0x2510,
	0x2514,	0x2534,	0x252C,	0x251C,	0x2500,	0x253C,	0x255E,	0x255F,	0x255A,	0x2554,	0x2569,	0x2566,	0x2560,	0x2550,	0x256C,	0x2567,
	0x2568,	0x2564,	0x2565,	0x2559,	0x2558,	0x2552,	0x2553,	0x256B,	0x256A,	0x2518,	0x250C,	0x2588,	0x2584,	0x258C,	0x2590,	0x2580
};

const unsigned short int _ir_internal_utf_866_symbols2[0x10] =
{
	0x401,	0x451,	0x404,	0x454,	0x407,	0x457,	0x40E,	0x45E,	0xB0,	0x2219,	0xB7,	0x221A,	0x2116,	0xA4,	0x25A0,	0xA0
};

unsigned int _ir_internal_utf_866_charsize(void)
{
	return 1;
}

unsigned int _ir_internal_utf_866_encode(unsigned int code, void *symbols, unsigned int errcode)
{
	unsigned char *s = (unsigned char*)symbols;

	if (code < 0x80)
	{
		if (s != NULL) s[0] = (unsigned char)code;
		return 1;
	}
	else if (code >= 0x410 && code < 0x440)
	{
		if (s != NULL) s[0] = (unsigned char)(code - 0x410 + 0x80);
		return 1;
	}
	else if (code >= 0x440 && code < 0x450)
	{
		if (s != NULL) s[0] = (unsigned char)(code - 0x440 + 0xE0);
		return 1;
	}
	else if (code >= 0xA0 && code <= 0x2593)
	{
		//can be optimized with binar search
		for (unsigned int i = 0; i < 0x30; i++)
		{
			if (_ir_internal_utf_866_symbols1[i] == code)
			{
				if (s != NULL) s[0] = (unsigned char)(0xB0 + i);
				return 1;
			}
		}

		for (unsigned int i = 0; i < 0x10; i++)
		{
			if (_ir_internal_utf_866_symbols2[i] == code)
			{
				if (s != NULL) s[0] = (unsigned char)(0xF0 + i);
				return 1;
			}
		}

		return _ir_internal_utf_866_encode(errcode, symbols, ' ');
	}
	else return _ir_internal_utf_866_encode(errcode, symbols, ' ');
}

unsigned int _ir_internal_utf_866_decode(const void *string, unsigned int *nsymbols)
{
	unsigned char c = *((unsigned char*)string);
	if (nsymbols != NULL) *nsymbols = 1;
	if (c < 0x80)
	{
		return c;
	}
	else if (c < 0xB0)
	{
		return c - 0x80 + 0x410;
	}
	else if (c < 0xE0)
	{
		return _ir_internal_utf_866_symbols1[c - 0xB0];
	}
	else if (c < 0xF0)
	{
		return c - 0xE0 + 0x440;
	}
	else
	{
		return _ir_internal_utf_866_symbols2[c - 0xF0];
	}
}

void _ir_internal_utf_866_free(void)
{
}

unsigned char _ir_internal_utf_866_init(void)
{
	ir_utf_866.charsize = _ir_internal_utf_866_charsize;
	ir_utf_866.encode = _ir_internal_utf_866_encode;
	ir_utf_866.decode = _ir_internal_utf_866_decode;
	ir_utf_866.free = _ir_internal_utf_866_free;
	return 1;
}

#ifdef _WIN32
	#pragma endregion
#endif

unsigned int ir_utf_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2, void *string2)
{
	const unsigned char *s1 = (const unsigned char *)string1;
	unsigned char *s2 = (unsigned char *)string2;
	unsigned int c1 = codec1->charsize();
	unsigned int c2 = codec2->charsize();
	unsigned int i1 = 0;
	unsigned int i2 = 0;

	while (1)
	{
		unsigned int n1;
		unsigned int code = codec1->decode(s1 + c1 * i1, &n1);
		unsigned int n2 = codec2->encode(code, (s2 == NULL) ? s2 : (s2 + c2 * i2), errcode);

		if (code == 0) return i2;
		
		i1 += n1;
		i2 += n2;
	}
}

void *ir_utf_alloc_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2)
{
	unsigned int size = codec2->charsize() * (ir_utf_recode(codec1, string1, errcode, codec2, NULL) + codec2->encode(0, NULL, errcode));
	void *result = malloc(size);
	if (result == NULL) return NULL;
	ir_utf_recode(codec1, string1, errcode, codec2, result);
	return result;
}

void *ir_utf_buffer_recode(struct ir_UTF_Codec *codec1, const void *string1, unsigned int errcode, struct ir_UTF_Codec *codec2)
{
	unsigned int size = codec2->charsize() * (ir_utf_recode(codec1, string1, errcode, codec2, NULL) + codec2->encode(0, NULL, errcode));
	if (_ir_internal_utf_buffer.data == NULL) _ir_internal_utf_buffer.data = malloc(size);
	else _ir_internal_utf_buffer.data = realloc(_ir_internal_utf_buffer.data, size);
	if (_ir_internal_utf_buffer.data == NULL) return NULL;
	ir_utf_recode(codec1, string1, errcode, codec2, _ir_internal_utf_buffer.data);
	return _ir_internal_utf_buffer.data;
}

//INIT AND FREE
void ir_utf_init(void)
{
	ir_utf_c.init = _ir_internal_utf_c_init;
	ir_utf_utf8.init = _ir_internal_utf_utf8_init;
	ir_utf_utf16.init = _ir_internal_utf_utf16_init;
	ir_utf_utf32.init = _ir_internal_utf_utf32_init;
	ir_utf_1251.init = _ir_internal_utf_1251_init;
	ir_utf_866.init = _ir_internal_utf_866_init;
}

void ir_utf_free(void)
{
	_ir_internal_utf_c_free();
	_ir_internal_utf_utf8_free();
	_ir_internal_utf_utf16_free();
	_ir_internal_utf_utf32_free();
	_ir_internal_utf_1251_free();
	_ir_internal_utf_866_free();
	if (_ir_internal_utf_buffer.data != NULL)
	{
		free(_ir_internal_utf_buffer.data);
		_ir_internal_utf_buffer.data = NULL;
		_ir_internal_utf_buffer.reserved = 0;
	}
}

#endif	//#ifndef IR_UTF_IMPLEMENTATION