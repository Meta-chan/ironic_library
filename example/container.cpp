#include "../include/ir/quiet_list.h"
#include "../include/ir/quiet_vector.h"
#include "../include/ir/vector.h"
#include "../include/ir/types.h"
#include <stdio.h>

template class ir::QuietList<ir::uint32>;
template class ir::QuietVector<ir::uint32>;
template class ir::Vector<ir::uint32>;

template<class V> bool test(V v, const char *name)
{
	printf("%s:\n", name);

	for (ir::uint32 i = 0; i < v.size(); i++)
	{
		v.push_back(i);
	}

	for (ir::uint32 i = 0; i < v.size(); i++)
	{
		printf("%u ", v[i]);
	}

	return 0;
}

int _main()
{
	ir::QuietList<ir::uint32> ql;
	test(ql, "QuietList");
	ir::QuietVector<ir::uint32> qv;
	test(qv, "QuietVector");
	ir::Vector<ir::uint32> v;
	test(v, "Vector");
	return 0;
}

int main()
{
	return _main();
}