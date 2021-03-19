/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

ir::Block::Block() noexcept : _data(nullptr), _size(0) {}
ir::Block::Block(const void *data, size_t size) noexcept : _data(data), _size(size) {}
const void *ir::Block::data() noexcept { return _data; }
size_t ir::Block::size() const noexcept { return _size; }
ir::Block::~Block() noexcept {}