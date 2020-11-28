#define IR_IMPLEMENT
#include <ir_parallel.h>
#include <stdio.h>

int _main()
{
	ir::Parallel parallel(8, 1000);
	parallel.parallel(nullptr, [](const void *, unsigned int id)
	{
		printf("Process %u here!\n", id);
	});
	printf("Finished\n");
	return 0;
}

int main()
{
	_main();
	getchar();
}