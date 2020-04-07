/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK_IMPLEMENTATION
#define IR_BLOCK_IMPLEMENTATION

#include <stdlib.h>

ir::BufferBlock::BufferBlock() : used(0), reserved(0), data(nullptr) {};

bool ir::BufferBlock::reserve(unsigned int newsize)
{
	if (reserved < newsize)
	{
		if (data == nullptr) data = (char*)malloc(newsize);
		else data = (char*)realloc(data, newsize);
		if (data == nullptr)
		{
			reserved = 0;
			used = 0;
			return false;
		}
		else reserved = newsize;
	}
	return true;
};

void ir::BufferBlock::free()
{
	if (data != nullptr) ::free(data);
	data = nullptr;
	used = 0;
	reserved = 0;
};

ir::Block::Block() : size(0), data(nullptr) {};
ir::Block::Block(unsigned int size, void *data) : size(size), data(data) {};
ir::Block::Block(BufferBlock bufferblock) : size(bufferblock.used), data(bufferblock.data) {};

ir::ConstBlock::ConstBlock() : size(0), data(nullptr) {};
ir::ConstBlock::ConstBlock(unsigned int size, const void *data) : size(size), data(data) {};
ir::ConstBlock::ConstBlock(Block block) : size(block.size), data(block.data) {};
ir::ConstBlock::ConstBlock(BufferBlock bufferblock) : size(bufferblock.used), data(bufferblock.data) {};

#endif	//#ifndef IR_BLOCK_IMPLEMENTATION