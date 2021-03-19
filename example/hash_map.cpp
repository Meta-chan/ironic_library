#include "../include/ir/quiet_hash_map.h"
#include <stdio.h>
#include <string>

class Hasher
{
public:
	static inline size_t hash(const std::string &s) noexcept
	{
		char h = 0;
		for (size_t i = 0; i < s.size(); i++)
			h ^= s[i];
		return h;
	}
};

template class ir::QuietHashMap<std::string, int, Hasher>;

int main()
{
	ir::QuietHashMap<std::string, int, Hasher> map;
	map.set("Sunflower", 1);
	map.set("Goddess", 2);
	map.set("Mercury", 3);
	printf("%i\n", map["Mercury"]);
}