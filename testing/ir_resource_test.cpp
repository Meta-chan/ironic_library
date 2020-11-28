#define IR_IMPLEMENT
#define _CRT_SECURE_NO_WARNINGS
#include <ir_resource/ir_resource.h>
#include <ir_resource/ir_file_resource.h>
#include <ir_resource/ir_memresource.h>
#include <stdlib.h>
#include <stdio.h>

class FloatIniterFreer
{
public:
	static float initvalue;
	static void free(float f) {}
};

float FloatIniterFreer::initvalue = 0;

int main()
{
	ir::Resource<float, FloatIniterFreer> floatres = 0;
	float f0 = 1 * floatres;

	ir::MemResource<float> res = ((float*)malloc(sizeof(float)));
	if (res == nullptr) return 0;
	f0 = res[0];
	res = nullptr;

	ir::FileResource fil = fopen("file.txt", "w");
	fwrite("file", 1, 4, fil);
	return 0;
}