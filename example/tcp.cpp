#define IR_INCLUDE 'a'
#include "../include/ir/tcp.h"
#include <stdio.h>


int main()
{
	printf("Type enter to follow server scenario\n");
	if (getchar() == '\n')
	{
		//Server scenario
		printf("Following server scenario\n");
		ir::TCPServer server(ir::IP(false, false, 5000));
		char buffer[256];
		server.accept();
		server.receive(0, buffer, 256);
		server.send(0, "Hello!", strlen("Hello!"));
	}
	else
	{
		//Client scenario
		printf("Following client scenario\n");
		ir::TCPClient client(false);
		char buffer[256];
		client.connect(ir::IP(false, false, 5000));
		client.send("Hello!", strlen("Hello!"));
		client.receive(buffer, strlen("Hello!"));
	}
	printf("Communication done!\n");
	return 0;
}