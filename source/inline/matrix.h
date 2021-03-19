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

template<class T, size_t A>
inline void ir::Chunk<T, A>::zero() noexcept
{
	for (size_t a = 0; a < A; a++) r[a] = (T)0;
}

template<class T, size_t A>
inline T ir::Chunk<T, A>::sum() const noexcept
{
	T s = 0.0;
	for (size_t a = 0; a < A; a++) s += r[a];
	return s;
}

template<class T, size_t A>
inline void ir::Chunk<T, A>::operator+=(const Chunk<T, A> & IR_RESTRICT another) noexcept
{
	for (size_t a = 0; a < A; a++) r[a] += another.r[a];
}

template<class T, size_t A>
inline void ir::Chunk<T, A>::operator-=(const Chunk<T, A> & IR_RESTRICT another) noexcept
{
	for (size_t a = 0; a < A; a++) r[a] -= another.r[a];
}

template<class T, size_t A>
inline void ir::Chunk<T, A>::operator*=(const Chunk<T, A> & IR_RESTRICT another) noexcept
{
	for (size_t a = 0; a < A; a++) r[a] *= another.r[a];
}

template<class T, size_t A>
inline void ir::Chunk<T, A>::operator/=(const Chunk<T, A> & IR_RESTRICT another) noexcept
{
	for (size_t a = 0; a < A; a++) r[a] /= another.r[a];
}

template<class T, size_t A>
inline ir::Chunk<T, A> ir::Chunk<T, A>::operator+(const Chunk<T, A> & IR_RESTRICT another) const noexcept
{
	Chunk<T, A> b;
	for (size_t a = 0; a < A; a++) b.r[a] = r[a] + another.r[a];
	return b;
}

template<class T, size_t A>
inline ir::Chunk<T, A> ir::Chunk<T, A>::operator-(const Chunk<T, A> & IR_RESTRICT another) const noexcept
{
	Chunk<T, A> b;
	for (size_t a = 0; a < A; a++) b.r[a] = r[a] - another.r[a];
	return b;
}

template<class T, size_t A>
inline ir::Chunk<T, A> ir::Chunk<T, A>::operator*(const Chunk<T, A> & IR_RESTRICT another) const noexcept
{
	Chunk<T, A> b;
	for (size_t a = 0; a < A; a++) b.r[a] = r[a] * another.r[a];
	return b;
}

template<class T, size_t A>
inline ir::Chunk<T, A> ir::Chunk<T, A>::operator/(const Chunk<T, A> & IR_RESTRICT another) const noexcept
{
	Chunk<T, A> b;
	for (size_t a = 0; a < A; a++) b.r[a] = r[a] / another.r[a];
	return b;
}

template <class T, size_t A>
inline size_t ir::Matrix<T, A>::width() const noexcept
{
	assert(_data != nullptr);
	return _width;
}

template <class T, size_t A>
inline size_t ir::Matrix<T, A>::height() const noexcept
{
	assert(_data != nullptr);
	return _height;
}

template <class T, size_t A>
inline T * IR_RESTRICT ir::Matrix<T, A>::data(size_t row) noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	T *aligned_data = (T*)(((size_t)_data + A * sizeof(T) - 1) / (A * sizeof(T)) * (A * sizeof(T)));
	size_t aligned_width = (_width + A - 1) / A * A;
	return aligned_data + row * aligned_width;
}

template <class T, size_t A>
inline const T * IR_RESTRICT ir::Matrix<T, A>::data(size_t row) const noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	const T *aligned_data = (const T*)(((size_t)_data + A * sizeof(T) - 1) / (A * sizeof(T)) * (A * sizeof(T)));
	size_t aligned_width = (_width + A - 1) / A * A;
	return aligned_data + row * aligned_width;
}

template <class T, size_t A>
inline T & IR_RESTRICT ir::Matrix<T, A>::at(size_t row, size_t column) noexcept
{
	assert(column < _width);
	return data(row)[column];
}

template <class T, size_t A>
inline const T & IR_RESTRICT ir::Matrix<T, A>::at(size_t row, size_t column) const noexcept
{
	assert(column < _width);
	return data(row)[column];
}

template <class T, size_t A>
inline T & IR_RESTRICT ir::Matrix<T, A>::operator()(size_t row, size_t column) noexcept
{
	assert(column < _width);
	return data(row)[column];
}

template <class T, size_t A>
inline const T & IR_RESTRICT ir::Matrix<T, A>::operator()(size_t row, size_t column) const noexcept
{
	assert(column < _width);
	return data(row)[column];
}

template <class T, size_t A>
inline ir::Chunk<T, A> * IR_RESTRICT ir::Matrix<T, A>::chunk_data(size_t row) noexcept
{
	return (Chunk<T, A>*)data(row);
}

template <class T, size_t A>
inline const ir::Chunk<T, A> * IR_RESTRICT ir::Matrix<T, A>::chunk_data(size_t row) const noexcept
{
	return (const Chunk<T, A>*)data(row);
}

template <class T, size_t A>
inline ir::Chunk<T, A> & IR_RESTRICT ir::Matrix<T, A>::chunk_at(size_t row, size_t column) noexcept
{
	assert(column < _width);
	assert(column % A == 0);
	return chunk_data(row)[column / A];
}

template <class T, size_t A>
inline const ir::Chunk<T, A> & IR_RESTRICT ir::Matrix<T, A>::chunk_at(size_t row, size_t column) const noexcept
{
	assert(column < _width);
	assert(column % A == 0);
	return chunk_data(row)[column / A];
}

template <class T, size_t A>
inline ir::Chunk<T, A> ir::Matrix<T, A>::vertical_chunk_at(size_t row, size_t column) const noexcept
{
	Chunk<T, A> b;
	for (size_t a = 0; a < A; a++) b.r[a] = at(row + a, column);
	return b;
}

template <class T, size_t A>
inline ir::Chunk<T, A> ir::Matrix<T, A>::vertical_chunk_at_edge(size_t row, size_t column) const noexcept
{
	Chunk<T, A> b;
	if (row + A <= _height)
	{
		for (size_t a = 0; a < A; a++) b.r[a] = at(row + a, column);
		return b;
	}
	else
	{
		memset(b.r, 0, A * sizeof(T));
		for (size_t a = 0; a < _height - row; a++) b.r[a] = at(row + a, column);
		return b;
	}
}