#define IR_INCLUDE 'a'
#include "../include/ir/udp.h"
#include <stdio.h>

int main()
{
	printf("Type enter to follow server scenario\n");
	if (getchar() == '\n')
	{
		//Server scenario
		printf("Following server scenario\n");
		ir::UDP server(ir::IP(false, false, 5000));
		char buffer[256];
		size_t size = 256;
		server.receive(nullptr, buffer, &size);
		server.send(ir::IP(false, false, 5001), "Hello!", strlen("Hello!"));
	}
	else
	{
		//Client scenario
		printf("Following client scenario\n");
		ir::UDP client(ir::IP(false, false, 5001));
		char buffer[256];
		size_t size = 256;
		client.send(ir::IP(false, false, 5000), "Hello!", strlen("Hello!"));
		client.receive(nullptr, buffer, &size);
	}
	printf("Communication done!\n");
	return 0;
}