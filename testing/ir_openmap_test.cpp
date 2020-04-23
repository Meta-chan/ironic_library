#define IR_IMPLEMENT
#include "ir_openmap.h"
#include <stdio.h>
#include <time.h>
#include <share.h>

#define SIZE 1024 * 1024

char destbuffer[SIZE];
char emulbuffer[SIZE];

int main()
{
	FILE *file = _wfsopen(L"text.hex", L"w+b", _SH_DENYNO);
	fwrite(destbuffer, 1, SIZE, file);

	ir::OpenmapCache cache;

	clock_t a = clock();
	for (unsigned int i = 0; i < 1000; i++)
	{
		fseek(file, 0, SEEK_SET);
		fread(emulbuffer, 1, SIZE, file);
		memcpy(destbuffer, emulbuffer, SIZE);
	}
	clock_t b = clock();
	for (unsigned int i = 0; i < 1000; i++)
	{
		void *map = ir::openmap(&cache, file, 0, SIZE, ir::openmapmode::openmap_read);
		memcpy(destbuffer, map, SIZE);
	}
	clock_t c = clock();
	ir::closemap(&cache);
	fclose(file);
	printf("fread time: %f\n", (float)(b - a) / CLOCKS_PER_SEC);
	printf("openmap time: %f\n", (float)(c - b) / CLOCKS_PER_SEC);
	getchar();
};