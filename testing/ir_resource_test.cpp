#define IR_IMPLEMENT
#define _CRT_SECURE_NO_WARNINGS
#include <ir_resource/ir_file_resource.h>
#include <ir_resource/ir_memresource.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
	ir::MemResource<float> res;
	res.it = (float*)malloc(sizeof(float));
	ir::FileResource fil(fopen("file.txt", "w"));
	fwrite("file", 1, 4, fil.it);
	return 0;
};