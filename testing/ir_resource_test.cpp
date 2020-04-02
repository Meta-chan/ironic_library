#define IR_IMPLEMENT
#include "ir_resource.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

class MemFreer
{
public:
	static void free(void *mem)
	{
		::free(mem);
	}
};

typedef ir::Resource<float*, MemFreer, nullptr> FloatResource;

class FileFreer
{
public:
	static void free(FILE *file)
	{
		::fclose(file);
	}
};

typedef ir::Resource<FILE*, FileFreer, nullptr> FileResource;

int main()
{
	FloatResource res;
	res.it = (float*)malloc(sizeof(float));
	FileResource fil(fopen("file.txt", "w"));
	fwrite("file", 1, 4, fil.it);
	return 0;
};