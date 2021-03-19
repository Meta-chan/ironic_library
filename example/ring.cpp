#include "../include/ir/quiet_ring.h"

int main()
{
	ir::QuietRing<int> ring(10);
	ring.write(1);
	ring.write(2);
	ring.write(3);
	int a;
	a = ring.read();
	a = ring.read();
	a = ring.read();
	return 0;
}