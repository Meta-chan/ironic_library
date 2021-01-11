#define IR_IMPLEMENT
#include <ir_container/ir_map.h>
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

template class ir::QuietMap<std::string, int, Comparator>;
template class ir::Map<std::string, int, Comparator>;

int main()
{
	ir::Map<std::string, int, Comparator> map;
	map.set("Sunflower", 1);
	map.set("Goddess", 2);
	map.set("Mercury", 3);
	printf("%i\n", map["Mercury"]);
}