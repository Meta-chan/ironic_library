#define IR_INCLUDE 'a'
#include "../include/ir/mapping.h"
#include "../include/ir/file.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
	#include <share.h>
#endif

#define BUFSIZE 421773
#define READSTART 69
#define READEND 420001

int _main()
{
	//Initialization
	void *destbuffer = malloc(BUFSIZE);
	void *emulbuffer = malloc(BUFSIZE);
	if (destbuffer == nullptr || emulbuffer == nullptr) return 1;
	ir::File file(SS("test.hex"), SS("w+b"));
	if (!file.ok()) return 1;
	file.write(destbuffer, BUFSIZE);

	//Ordinar read
	clock_t a = clock();
	for (ir::uint32 i = 0; i < 10; i++)
	{
		file.seek(READSTART, SEEK_SET);
		if (file.read(emulbuffer, READEND - READSTART) < (READEND - READSTART)) return 1;
		memcpy(destbuffer, emulbuffer, BUFSIZE);
	}

	//Mapping read
	clock_t b = clock();
	ir::Mapping mapping;
	for (ir::uint32 i = 0; i < 10; i++)
	{
		void *map = mapping.map(file.file(), READSTART, READEND - READSTART, ir::Mapping::map_mode::read);
		if (map == nullptr) return 1;
		memcpy(destbuffer, map, BUFSIZE);
	}

	//Printing results
	clock_t c = clock();
	printf("fread time: %f\n", (float)(b - a) / CLOCKS_PER_SEC);
	printf("openmap time: %f\n", (float)(c - b) / CLOCKS_PER_SEC);
	getchar();
	return 0;
}

int main()
{
	return _main();
}
