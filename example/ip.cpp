#define IR_INCLUDE 'a'
#include "../include/ir/ip.h"

int main()
{
	ir::IP ip4(true, false, 1000);
	ir::IP ip6(true, true, 1000);
}