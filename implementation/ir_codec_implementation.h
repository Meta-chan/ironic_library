/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_CODEC_IMPLEMENTATION
#define IR_CODEC_IMPLEMENTATION

#include <stdlib.h>

#ifdef _WIN32
	#pragma region ASCII
#endif

ir::QuietVector<char> ir::Codec::_buffer;

bool ir::Codec::ASCII::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	unsigned char *s = (unsigned char*)string;
	if (code < 0x80)
	{
		if (s != nullptr) s[0] = (unsigned char)code;
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else return false;
}

size_t ir::Codec::ASCII::decode(const Char *string, size_t *nnative) noexcept
{
	unsigned char c = *((unsigned char*)string);
	if (c < 0x80)
	{
		if (nnative != nullptr) *nnative = 1;
		return c;
	}
	else
	{
		*nnative = 0;
		return 0;
	}
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF8
#endif

bool ir::Codec::UTF8::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	unsigned char *s = (unsigned char*)string;
	if (code < 0x80)
	{
		if (s != nullptr) s[0] = (unsigned char)code;
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code < 0x800)
	{
		if (s != nullptr)
		{
			s[0] = 0xC0 | ((code >> 6) & 0x1F);
			s[1] = 0x80 | (code & 0x3F);
		}
		if (nnative != nullptr) *nnative = 2;
		return true;
	}
	else if (code < 0x10000)
	{
		if (s != nullptr)
		{
			s[0] = 0xE0 | ((code >> 12) & 0x0F);
			s[1] = 0x80 | ((code >> 6) & 0x3F);
			s[2] = 0x80 | (code & 0x3F);
		}
		if (nnative != nullptr) *nnative = 3;
		return true;
	}
	else if (code < 0x110000)
	{
		if (s != nullptr)
		{
			s[0] = 0xE0 | ((code >> 18) & 0x07);
			s[1] = 0x80 | ((code >> 12) & 0x3F);
			s[2] = 0x80 | ((code >> 6) & 0x3F);
			s[3] = 0x80 | (code & 0x3F);
		}
		if (nnative != nullptr) *nnative = 4;
		return true;
	}
	else return false;
}

size_t ir::Codec::UTF8::decode(const Char *string, size_t *nnative) noexcept
{
	const unsigned char *s = (const unsigned char*)string;
	if ((s[0] & 0x80) == 0)
	{
		if (nnative != nullptr) *nnative = 1;
		return s[0];
	}
	else if ((s[0] & 0xE0) == 0xC0 && (s[1] & 0xC0) == 0x80)
	{
		if (nnative != nullptr) *nnative = 2;
		return ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
	}
	else if ((s[0] & 0xF0) == 0xE0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
	{
		if (nnative != nullptr) *nnative = 3;
		return ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
	}
	else if ((s[0] & 0xF8) == 0xF0 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80)
	{
		if (nnative != nullptr) *nnative = 4;
		return ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
	}
	else
	{
		*nnative = 0;
		return 0;
	}
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF16
#endif

bool ir::Codec::UTF16::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	unsigned short int *s = (unsigned short int*)string;
	if (code < 0x10000 && !(code >= 0xD800 && code < 0xE000))
	{
		if (s != nullptr) s[0] = (unsigned short)code;
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code < 0x110000)
	{
		if (s != nullptr)
		{
			s[0] = ((code - 0x10000) >> 10) & 0x3FF;
			s[1] = (code - 0x10000) & 0x3FF;
		}
		if (nnative != nullptr) *nnative = 2;
		return true;
	}
	else return false;
}

size_t ir::Codec::UTF16::decode(const Char *string, size_t *nnative) noexcept
{
	const unsigned short int *s = (const unsigned short int*)string;
	if (!(s[0] >= 0xD800 && s[0] < 0xE000))
	{
		if (nnative != nullptr) *nnative = 1;
		return s[0];
	}
	else if ((s[0] >= 0xD800 && s[0] < 0xDC00) && (s[1] >= 0xDC00 && s[1] < 0xE000))
	{
		if (nnative != nullptr) *nnative = 2;
		return 0;
	}
	else
	{
		*nnative = 0;
		return 0;
	}
}

#ifdef _WIN32
	#pragma endregion
	#pragma region UTF32
#endif

bool ir::Codec::UTF32::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	if (string != nullptr) *((unsigned int*)string) = code;
	if (nnative != nullptr) *nnative = 1;
	return true;
}

size_t ir::Codec::UTF32::decode(const Char *string, size_t *nnative) noexcept
{
	if (nnative != nullptr) *nnative = 1;
	return *((unsigned int*) nnative);
}

#ifdef _WIN32
	#pragma endregion
	#pragma region 1251
#endif

const unsigned short int ir::Codec::CP1251::_decode_table[0x40] =
{
	0x402,	0x403,	0x201A,	0x453,	0x201E,	0x2026,	0x2020,	0x2021,	0x20AC,	0x2030,	0x409,	0x2039,	0x40A,	0x40C,	0x40B,	0x40F,
	0x452,	0x2018,	0x2019,	0x201C,	0x201D,	0x2022,	0x2013,	0x2014,	0x0,	0x2122,	0x459,	0x203A,	0x45A,	0x45C,	0x45B,	0x45F,
	0xA0,	0x40E,	0x45E,	0x408,	0xA4,	0x490,	0xA6,	0xA7,	0x401,	0xA9,	0x404,	0xAB,	0xAC,	0xAD,	0xAE,	0x407,
	0xB0,	0xB1,	0x406,	0x456,	0x491,	0xB5,	0xB6,	0xB7,	0x451,	0x2116,	0x454,	0xBB,	0x458,	0x405,	0x455,	0x457
};

bool ir::Codec::CP1251::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	unsigned char *s = (unsigned char*)string;
	
	if (code < 0x80)
	{
		if (s != nullptr) s[0] = (unsigned char)code;
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code >= 0x410 && code < 0x450)
	{
		if (s != nullptr) s[0] = (unsigned char)(code - 0x410 + 0xC0);
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code >= 0xA0 && code <= 0x2122)
	{
		for (unsigned int i = 0; i < 0x40; i++)
		{
			//can be optimized with binar search
			if (_decode_table[i] == code)
			{
				if (s != nullptr) s[0] = (unsigned char)(0x80 + i);
				if (nnative != nullptr) *nnative = 1;
				return true;
			}
		}
		return false;
	}
	else return false;
}

size_t ir::Codec::CP1251::decode(const Char *string, size_t *nnative) noexcept
{
	unsigned char c = *((unsigned char*)string);
	if (c < 0x80)
	{
		if (nnative != nullptr) *nnative = 1;
		return c;
	}
	else if (c < 0xC0)
	{
		if (nnative != nullptr) *nnative = 1;
		return _decode_table[c - 0x80];
	}
	else if (c != 0x98)
	{
		if (nnative != nullptr) *nnative = 1;
		return c - 0xC0 + 0x410;
	}
	else
	{
		*nnative = 0;
		return 0;
	}
}

#ifdef _WIN32
	#pragma endregion
	#pragma region 866
#endif

const unsigned short int ir::Codec::CP866::_decode_table1[0x30] =
{
	0x2591,	0x2592,	0x2593,	0x2502,	0x2524,	0x2561,	0x2562,	0x2556,	0x2555,	0x2563,	0x2551,	0x2557,	0x255D,	0x255C,	0x255B,	0x2510,
	0x2514,	0x2534,	0x252C,	0x251C,	0x2500,	0x253C,	0x255E,	0x255F,	0x255A,	0x2554,	0x2569,	0x2566,	0x2560,	0x2550,	0x256C,	0x2567,
	0x2568,	0x2564,	0x2565,	0x2559,	0x2558,	0x2552,	0x2553,	0x256B,	0x256A,	0x2518,	0x250C,	0x2588,	0x2584,	0x258C,	0x2590,	0x2580
};

const unsigned short int ir::Codec::CP866::_decode_table2[0x10] =
{
	0x401,	0x451,	0x404,	0x454,	0x407,	0x457,	0x40E,	0x45E,	0xB0,	0x2219,	0xB7,	0x221A,	0x2116,	0xA4,	0x25A0,	0xA0
};

bool ir::Codec::CP866::encode(unsigned int code, Char *string, size_t *nnative) noexcept
{
	unsigned char *s = (unsigned char*)string;

	if (code < 0x80)
	{
		if (s != nullptr) s[0] = (unsigned char)code;
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code >= 0x410 && code < 0x440)
	{
		if (s != nullptr) s[0] = (unsigned char)(code - 0x410 + 0x80);
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code >= 0x440 && code < 0x450)
	{
		if (s != nullptr) s[0] = (unsigned char)(code - 0x440 + 0xE0);
		if (nnative != nullptr) *nnative = 1;
		return true;
	}
	else if (code >= 0xA0 && code <= 0x2593)
	{
		//can be optimized with binar search
		for (unsigned int i = 0; i < 0x30; i++)
		{
			if (_decode_table1[i] == code)
			{
				if (s != nullptr) s[0] = (unsigned char)(0xB0 + i);
				if (nnative != nullptr) *nnative = 1;
				return true;
			}
		}

		for (unsigned int i = 0; i < 0x10; i++)
		{
			if (_decode_table2[i] == code)
			{
				if (s != nullptr) s[0] = (unsigned char)(0xF0 + i);
				if (nnative != nullptr) *nnative = 1;
				return true;
			}
		}
		return false;
	}
	else return false;
}

size_t ir::Codec::CP866::decode(const Char *string, size_t *nnative) noexcept
{
	unsigned char c = *((unsigned char*)string);
	if (nnative != nullptr) *nnative = 1;
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
		return _decode_table1[c - 0xB0];
	}
	else if (c < 0xF0)
	{
		return c - 0xE0 + 0x440;
	}
	else
	{
		return _decode_table2[c - 0xF0];
	}
}

#ifdef _WIN32
	#pragma endregion
#endif

template<class SourceCodec, class DestCodec>
size_t ir::Codec::recode(const typename SourceCodec::Char *source, unsigned int errcode, typename DestCodec::Char *dest) noexcept
{
	size_t isource = 0;
	size_t idest = 0;

	while (true)
	{
		size_t nsource;
		unsigned int code = SourceCodec::decode(&source[isource], &nsource);
		size_t ndest;
		if (!DestCodec::encode(code, (dest == nullptr) ? dest : &dest[idest], &ndest))
			DestCodec::encode(errcode, (dest == nullptr) ? dest : &dest[idest], &ndest);

		if (code == 0) return idest;
		
		isource += nsource;
		idest += ndest;
	}
}

template<class SourceCodec, class DestCodec>
typename DestCodec::Char *ir::Codec::alloc_recode(const typename SourceCodec::Char *source, unsigned int errcode) noexcept
{
	size_t size = (recode<SourceCodec, DestCodec>(source, errcode, nullptr) + 1) * sizeof(DestCodec::Char);
	DestCodec::Char *result = (DestCodec::Char*)malloc(size);
	if (result == nullptr) return nullptr;
	recode<SourceCodec, DestCodec>(source, errcode, result);
	return result;
}

template<class SourceCodec, class DestCodec>
typename DestCodec::Char *ir::Codec::buffer_recode(const typename SourceCodec::Char *source, unsigned int errcode) noexcept
{
	size_t size = (recode<SourceCodec, DestCodec>(source, errcode, nullptr) + 1) * sizeof(DestCodec::Char);
	if (!_buffer.resize(size)) return nullptr;
	recode<SourceCodec, DestCodec>(source, errcode, (DestCodec::Char*)&_buffer[0]);
	return (DestCodec::Char*)&_buffer[0];
}

void ir::Codec::clear() noexcept
{
	_buffer.clear();
}

#endif	//#ifndef IR_CODEC_IMPLEMENTATION