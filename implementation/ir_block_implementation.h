/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK_IMPLEMENTATION
#define IR_BLOCK_IMPLEMENTATION

ir::Block::Block() : size(0), data(nullptr) {};
ir::Block::Block(unsigned int size, void *data) : size(size), data(data) {};

ir::ConstBlock::ConstBlock() : size(0), data(nullptr) {};
ir::ConstBlock::ConstBlock(unsigned int size, const void *data) : size(size), data(data) {};
ir::ConstBlock::ConstBlock(Block block) : size(block.size), data(block.data) {};

#endif	//#ifndef IR_BLOCK_IMPLEMENTATION