#define IR_IMPLEMENT
#include <ir_openmap.h>
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

int main()
{
	void *destbuffer = malloc(BUFSIZE);
	void *emulbuffer = malloc(BUFSIZE);
	if (destbuffer == nullptr || emulbuffer == nullptr) return 1;

	#ifdef _WIN32
		FILE *file = _wfsopen(L"test.hex", L"w+b", _SH_DENYNO);
	#else
		FILE *file = fopen("test.hex", "w+b");
	#endif
	if (file == nullptr) return 1;
	fwrite(destbuffer, 1, BUFSIZE, file);

	ir::OpenmapCache cache;

	clock_t a = clock();
	for (unsigned int i = 0; i < 10; i++)
	{
		fseek(file, READSTART, SEEK_SET);
		if (fread(emulbuffer, 1, READEND - READSTART, file) < (READEND - READSTART)) return 1;
		memcpy(destbuffer, emulbuffer, BUFSIZE);
	}
	clock_t b = clock();
	for (unsigned int i = 0; i < 10; i++)
	{
		void *map = ir::openmap(&cache, file, READSTART, READEND - READSTART, ir::openmap_mode::read);
		if (map == nullptr) return 1;
		memcpy(destbuffer, map, BUFSIZE);
	}
	clock_t c = clock();
	ir::closemap(&cache);
	fclose(file);
	printf("fread time: %f\n", (float)(b - a) / CLOCKS_PER_SEC);
	printf("openmap time: %f\n", (float)(c - b) / CLOCKS_PER_SEC);
	getchar();
}
