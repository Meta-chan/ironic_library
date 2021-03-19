#include "../include/ir/quiet_map.h"
#include "../include/ir/map.h"
#include "../include/ir/quiet_hash_map.h"
#include <stdio.h>
#include <string>

class Comparator
{
public:
	static inline int compare(const std::string &s1, const std::string &s2) noexcept
	{
		if (s1.size() > s2.size()) return 1;
		else if (s1.size() < s2.size()) return -1;
		for (size_t i = 0; i < s1.size(); i++)
		{
			if (s1[i] != s2[i]) return (s1[i] - s2[i]);
		}
		return 0;
	}
};

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

template class ir::QuietMap<std::string, int, Comparator>;
template class ir::Map<std::string, int, Comparator>;
template class ir::QuietHashMap<std::string, std::string, Hasher>;

template <class M> void test(M m, const char *name)
{
	m.set("Honesty", "Applejack");
	m.set("Kindness", "Fluttershy");
	m.set("Laughter", "Pinkie Pie");
	m.set("Generosity", "Rarity");
	m.set("Loyalty", "Rainbow Dash");
	m.set("Magic", "Twilight");

	m["Laughter"] = "Silver Bell";
	m["Honesty"] = "Monterey Jack";
	m["Loyalty"] = "Dead Eye";
	m["Generosity"] = "Red Eye";
	m["Magic"] = "Goddess";
	m.erase("Kindness");
	printf("%s: %s\n", name, m["Magic"].data());
}

int _main()
{
	ir::QuietMap<std::string, std::string, Comparator> qm;
	test(qm, "QuietMap");
	ir::Map<std::string, std::string, Comparator> m;
	test(m, "Map");
	ir::QuietHashMap<std::string, std::string, Hasher> qhm;
	test(qhm, "QuietMap");
	return 0;
}

int main()
{
	return _main();
}