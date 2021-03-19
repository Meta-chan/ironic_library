/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

ir::uint32 ir::fnv1a(Block key) noexcept
{
	uint32 h = 2166136261;
	const char *keydata = (const char*)key.data();
	size_t keysize = key.size();
	for (size_t i = 0; i < keysize; i++)
	{
		uint32 octet = keydata[i];
		h = h ^ octet;
		h = h * 16777619;
	}
	return h;
}