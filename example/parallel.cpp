#define IR_INCLUDE 'a'
#define IR_PARALLEL_IMPLEMENTATION 'o'
#include "../include/ir/parallel.h"
#include <stdio.h>

int _main()
{
	ir::Parallel::init(8);
	ir::Parallel::parallel(nullptr, [](const void *, ir::uint32 id, ir::uint32 n)
	{
		printf("Process %u here!\n", id);
	});
	printf("Finished\n");
	getchar();
	return 0;
}

int main()
{
	return _main();
}