#include "../include/ir/resource.h"
#include <stdlib.h>

int _main()
{
	ir::Resource<int*> memory([](int*&r) { if (r == nullptr) return; free(r); r = nullptr; });
	memory = (int*)malloc(42 * sizeof(int));
	return 0;
}

int main()
{
	return _main();
}