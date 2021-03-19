#define IR_INCLUDE 'a'
#include "../include/ir/md5.h"
#include "../include/ir/types.h"

int main()
{
	ir::uint32 hash[4];
	ir::md5("", 0, hash);
	return 0;
}