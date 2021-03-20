/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifdef _WIN32
	#pragma region ASCII
#endif

inline bool ir::Encoding::ASCII::encode(uint32 code, Char *string, size_t *nnative) noexcept
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

inline ir::uint32 ir::Encoding::ASCII::decode(const Char *string, size_t *nnative) noexcept
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

inline bool ir::Encoding::UTF8::encode(uint32 code, Char *string, size_t *nnative) noexcept
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

inline ir::uint32 ir::Encoding::UTF8::decode(const Char *string, size_t *nnative) noexcept
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

inline bool ir::Encoding::UTF16::encode(uint32 code, Char *string, size_t *nnative) noexcept
{
	uint16 *s = (uint16*)string;
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

inline ir::uint32 ir::Encoding::UTF16::decode(const Char *string, size_t *nnative) noexcept
{
	const uint16 *s = (const uint16*)string;
	if (!(s[0] >= 0xD800 && s[0] < 0xE000))
	{
		if (nnative != nullptr) *nnative = 1;
		return s[0];
	}
	else if ((s[0] >= 0xD800 && s[0] < 0xDC00) && (s[1] >= 0xDC00 && s[1] < 0xE000))
	{
		if (nnative != nullptr) *nnative = 2;
		return (((s[0] - 0xD800) << 10) | (s[1] - 0xDC00)) + 0x10000;
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

inline bool ir::Encoding::UTF32::encode(uint32 code, Char *string, size_t *nnative) noexcept
{
	if (string != nullptr) *((uint32*)string) = code;
	if (nnative != nullptr) *nnative = 1;
	return true;
}

inline ir::uint32 ir::Encoding::UTF32::decode(const Char *string, size_t *nnative) noexcept
{
	if (nnative != nullptr) *nnative = 1;
	return *((uint32*) string);
}

#ifdef _WIN32
	#pragma endregion
	#pragma region 1251
#endif

inline bool ir::Encoding::CP1251::encode(uint32 code, Char *string, size_t *nnative) noexcept
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
		for (uint32 i = 0; i < 0x40; i++)
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

inline ir::uint32 ir::Encoding::CP1251::decode(const Char *string, size_t *nnative) noexcept
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

inline bool ir::Encoding::CP866::encode(uint32 code, Char *string, size_t *nnative) noexcept
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
		for (uint32 i = 0; i < 0x30; i++)
		{
			if (_decode_table1[i] == code)
			{
				if (s != nullptr) s[0] = (unsigned char)(0xB0 + i);
				if (nnative != nullptr) *nnative = 1;
				return true;
			}
		}

		for (uint32 i = 0; i < 0x10; i++)
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

inline ir::uint32 ir::Encoding::CP866::decode(const Char *string, size_t *nnative) noexcept
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
