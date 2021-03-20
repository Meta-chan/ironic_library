/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <assert.h>

template<class T>
inline void ir::QuietRing<T>::write(size_t count, const T *data) noexcept
{
	Block b = write(count);
	for (size_t i = 0; i < b.size[0]; i++) b.data[0][i] = data[i];
	for (size_t i = 0; i < b.size[1]; i++) b.data[1][i] = data[b.size[0] + i];
}

template<class T>
inline void ir::QuietRing<T>::write(const T &data) noexcept
{
	Block b = direct_write(1);
	b.data[0][0] = data;
}

template<class T>
inline void ir::QuietRing<T>::read(size_t count, T *data) const noexcept
{
	Block b = direct_read(count);
	for (size_t i = 0; i < b.size[0]; i++) data[i] = b.data[0][i];
	for (size_t i = 0; i < b.size[1]; i++) data[b.size[0] + i] = b.data[1][i];
}

template<class T>
inline T ir::QuietRing<T>::read() const noexcept
{
	Block b = direct_read(1);
	return b.data[0][0];
}

template<class T>
inline size_t ir::QuietRing<T>::count() const noexcept
{
	assert(_data != nullptr);
	return _count;
}

template<class T>
inline size_t ir::QuietRing<T>::size() const noexcept
{
	assert(_data != nullptr);
	return _size;
}