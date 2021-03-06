/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

ir::QuietVector<char> ir::Encoding::_buffer;

const ir::uint16 ir::Encoding::CP1251::_decode_table[0x40] =
{
	0x402,	0x403,	0x201A,	0x453,	0x201E,	0x2026,	0x2020,	0x2021,	0x20AC,	0x2030,	0x409,	0x2039,	0x40A,	0x40C,	0x40B,	0x40F,
	0x452,	0x2018,	0x2019,	0x201C,	0x201D,	0x2022,	0x2013,	0x2014,	0x0,	0x2122,	0x459,	0x203A,	0x45A,	0x45C,	0x45B,	0x45F,
	0xA0,	0x40E,	0x45E,	0x408,	0xA4,	0x490,	0xA6,	0xA7,	0x401,	0xA9,	0x404,	0xAB,	0xAC,	0xAD,	0xAE,	0x407,
	0xB0,	0xB1,	0x406,	0x456,	0x491,	0xB5,	0xB6,	0xB7,	0x451,	0x2116,	0x454,	0xBB,	0x458,	0x405,	0x455,	0x457
};

const ir::uint16 ir::Encoding::CP866::_decode_table1[0x30] =
{
	0x2591,	0x2592,	0x2593,	0x2502,	0x2524,	0x2561,	0x2562,	0x2556,	0x2555,	0x2563,	0x2551,	0x2557,	0x255D,	0x255C,	0x255B,	0x2510,
	0x2514,	0x2534,	0x252C,	0x251C,	0x2500,	0x253C,	0x255E,	0x255F,	0x255A,	0x2554,	0x2569,	0x2566,	0x2560,	0x2550,	0x256C,	0x2567,
	0x2568,	0x2564,	0x2565,	0x2559,	0x2558,	0x2552,	0x2553,	0x256B,	0x256A,	0x2518,	0x250C,	0x2588,	0x2584,	0x258C,	0x2590,	0x2580
};

const ir::uint16 ir::Encoding::CP866::_decode_table2[0x10] =
{
	0x401,	0x451,	0x404,	0x454,	0x407,	0x457,	0x40E,	0x45E,	0xB0,	0x2219,	0xB7,	0x221A,	0x2116,	0xA4,	0x25A0,	0xA0
};

void ir::Encoding::clear() noexcept
{
	_buffer.clear();
}