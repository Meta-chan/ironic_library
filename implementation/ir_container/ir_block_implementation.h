/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_BLOCK_IMPLEMENTATION
#define IR_BLOCK_IMPLEMENTATION

ir::ConstBlock::ConstBlock() noexcept : _data(nullptr), _size(0) {}
ir::ConstBlock::ConstBlock(const void *data, size_t size) noexcept : _data(data), _size(size) {}
const void *ir::ConstBlock::data() noexcept { return _data; }
size_t ir::ConstBlock::size() const noexcept { return _size; }
ir::ConstBlock::~ConstBlock() noexcept {}

#endif	//#ifndef IR_BLOCK_IMPLEMENTATION