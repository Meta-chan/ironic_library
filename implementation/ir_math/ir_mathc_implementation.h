/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MATHC_IMPLEMENTATION
#define IR_MATHC_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>
#include <random>

#ifndef restrict
	#ifdef __cplusplus
		#define restrict __restrict
	#endif
#endif

template<class T, unsigned int A>
inline T ir::BlockC<T, A>::sum() const noexcept
{
	T s = 0.0;
	for (unsigned int a = 0; a < A; a++) s += r[a];
	return s;
}

template<class T, unsigned int A>
inline void ir::BlockC<T, A>::assign_zero() noexcept
{
	for (unsigned int a = 0; a < A; a++) r[a] = 0.0;
}

template<class T, unsigned int A>
inline void ir::BlockC<T, A>::operator+=(const BlockC<T, A> & restrict another) noexcept
{
	for (unsigned int a = 0; a < A; a++) r[a] += another.r[a];
}

template<class T, unsigned int A>
inline void ir::BlockC<T, A>::operator-=(const BlockC<T, A> & restrict another) noexcept
{
	for (unsigned int a = 0; a < A; a++) r[a] -= another.r[a];
}

template<class T, unsigned int A>
inline void ir::BlockC<T, A>::operator*=(const BlockC<T, A> & restrict another) noexcept
{
	for (unsigned int a = 0; a < A; a++) r[a] *= another.r[a];
}

template<class T, unsigned int A>
inline void ir::BlockC<T, A>::operator/=(const BlockC<T, A> & restrict another) noexcept
{
	for (unsigned int a = 0; a < A; a++) r[a] /= another.r[a];
}

template<class T, unsigned int A>
inline ir::BlockC<T, A> ir::BlockC<T, A>::operator+(const BlockC<T, A> & restrict another) const noexcept
{
	BlockC<T, A> b;
	for (unsigned int a = 0; a < A; a++) b.r[a] = r[a] + another.r[a];
	return b;
}

template<class T, unsigned int A>
inline ir::BlockC<T, A> ir::BlockC<T, A>::operator-(const BlockC<T, A> & restrict another) const noexcept
{
	BlockC<T, A> b;
	for (unsigned int a = 0; a < A; a++) b.r[a] = r[a] - another.r[a];
	return b;
}

template<class T, unsigned int A>
inline ir::BlockC<T, A> ir::BlockC<T, A>::operator*(const BlockC<T, A> & restrict another) const noexcept
{
	BlockC<T, A> b;
	for (unsigned int a = 0; a < A; a++) b.r[a] = r[a] * another.r[a];
	return b;
}

template<class T, unsigned int A>
inline ir::BlockC<T, A> ir::BlockC<T, A>::operator/(const BlockC<T, A> & restrict another) const noexcept
{
	BlockC<T, A> b;
	for (unsigned int a = 0; a < A; a++) b.r[a] = r[a] / another.r[a];
	return b;
}

template <class T, unsigned int A>
ir::VectorC<T, A>::VectorC(unsigned int height, bool *ok) noexcept
{
	unsigned int size = (((height + A - 1) / A) * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr)
	{
		memset(_data, 0, size);
		_height = height;
	}
	if (ok != nullptr) *ok = (_data != nullptr);
}

template <class T, unsigned int A>
unsigned int ir::VectorC<T, A>::height() const noexcept
{
	assert(_data != nullptr);
	return _height;
}

template <class T, unsigned int A>
T * restrict ir::VectorC<T, A>::data() noexcept
{
	assert(_data != nullptr);
	return (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
}

template <class T, unsigned int A>
const T * restrict ir::VectorC<T, A>::data() const noexcept
{
	assert(_data != nullptr);
	return (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
}

template <class T, unsigned int A>
inline T & restrict ir::VectorC<T, A>::at(unsigned int row) noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	return data()[row];
}

template <class T, unsigned int A>
inline const T & restrict ir::VectorC<T, A>::at(unsigned int row) const noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	return data()[row];
}

template <class T, unsigned int A>
unsigned int ir::VectorC<T, A>::block_height() const noexcept
{
	assert(_data != nullptr);
	return (_height + A - 1) / A;
}

template <class T, unsigned int A>
ir::BlockC<T, A> * restrict ir::VectorC<T, A>::block_data() noexcept
{
	return (BlockC<T, A>*)data();
}

template <class T, unsigned int A>
const ir::BlockC<T, A> * restrict ir::VectorC<T, A>::block_data() const noexcept
{
	return (BlockC<T, A>*)data();
}

template <class T, unsigned int A>
inline ir::BlockC<T, A> & restrict ir::VectorC<T, A>::block(unsigned int rowblock) noexcept
{
	assert(_data != nullptr);
	assert(rowblock < block_height());
	return block_data()[rowblock];
}

template <class T, unsigned int A>
inline const ir::BlockC<T, A> & restrict ir::VectorC<T, A>::block(unsigned int rowblock) const noexcept
{
	assert(_data != nullptr);
	assert(rowblock < block_height());
	return block_data()[rowblock];
}

template<class T, unsigned int A>
void ir::VectorC<T, A>::assign_zero()
{
	for (unsigned int i = 0; i < _height; i++)
	{
		at(i) = 0;
	}
}

template<class T, unsigned int A>
void ir::VectorC<T, A>::assign_random(T low, T high)
{
	std::default_random_engine engine;
	std::uniform_real_distribution<T> distribution(low, high);
	for (unsigned int i = 0; i < _height; i++)
	{
		at(i) = distribution(engine);
	}
}

template<class T, unsigned int A>
void ir::VectorC<T, A>::assign_add(const VectorC<T, A> * restrict a, const VectorC<T, A> * restrict b) noexcept
{
	assert(this != nullptr			&& a != nullptr			&& b != nullptr);
	assert(_data != nullptr			&& a->_data != nullptr	&& b->_data != nullptr);
	assert(_data != a->_data		&& _data != b->_data	&& a->_data != b->_data);
	assert(height() == a->height()	&& a->height() == b->height());

	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b)
	#endif
	for (int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		block(lineblock) = a->block(lineblock) + b->block(lineblock);
	}
}

template<class T, unsigned int A>
void ir::VectorC<T, A>::assign_sub(const VectorC<T, A> * restrict a, const VectorC<T, A> * restrict b) noexcept
{
	assert(this != nullptr			&& a != nullptr			&& b != nullptr);
	assert(_data != nullptr			&& a->_data != nullptr	&& b->_data != nullptr);
	assert(_data != a->_data		&& _data != b->_data	&& a->_data != b->_data);
	assert(height() == a->height()	&& a->height() == b->height());

	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b)
	#endif
	for (int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		block(lineblock) = a->block(lineblock) - b->block(lineblock);
	}
}

template<class T, unsigned int A>
void ir::VectorC<T, A>::assign_mul(const MatrixC<T, A> * restrict a, const VectorC<T, A> * restrict b) noexcept
{
	assert(this != nullptr			&& a != nullptr			&& b != nullptr);
	assert(_data != nullptr			&& a->_data != nullptr	&& b->_data != nullptr);
	assert(_data != a->_data		&& _data != b->_data	&& a->_data != b->_data);
	assert(height() == a->height()	&& a->width() == b->height());

	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b)
	#endif
	for (int row = 0; (unsigned int)row < height(); row++)
	{
		BlockC<T, A> sum;
		sum.assign_zero();
		for (unsigned int columnblock = 0; columnblock < a->block_width(); columnblock++)
		{
			sum += a->block(row, columnblock) * b->block(columnblock);
		}
		at(row) = sum.sum();
	}
}

template <class T, unsigned int A>
ir::VectorC<T, A>::~VectorC() noexcept
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
}

template <class T, unsigned int A>
ir::MatrixC<T, A>::MatrixC(unsigned int width, unsigned int height, bool *ok) noexcept
{
	unsigned int size = (height * ((width + A - 1) / A) * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr)
	{
		memset(_data, 0, size);
		_height = height;
		_width = width;
	}
	if (ok != nullptr) *ok = (_data != nullptr);
}

template <class T, unsigned int A>
unsigned int ir::MatrixC<T, A>::width() const noexcept
{
	return _width;
}

template <class T, unsigned int A>
unsigned int ir::MatrixC<T, A>::height() const noexcept
{
	return _height;
}

template <class T, unsigned int A>
T * restrict ir::MatrixC<T, A>::data(unsigned int row) noexcept
{
	assert(_data != nullptr);
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + row * block_width() * A;
}

template <class T, unsigned int A>
const T * restrict ir::MatrixC<T, A>::data(unsigned int row) const noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + row * block_width() * A;
}

template <class T, unsigned int A>
inline T & restrict ir::MatrixC<T, A>::at(unsigned int row, unsigned int column) noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	assert(column < _width);
	return data(row)[column];
}

template <class T, unsigned int A>
inline const T & restrict ir::MatrixC<T, A>::at(unsigned int row, unsigned int column) const noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	assert(column < _width);
	return data(row)[column];
}

template <class T, unsigned int A>
unsigned int ir::MatrixC<T, A>::block_width() const noexcept
{
	assert(_data != nullptr);
	return (_width + A - 1) / A;
}

template <class T, unsigned int A>
ir::BlockC<T, A> * restrict ir::MatrixC<T, A>::block_data(unsigned int row) noexcept
{
	assert(_data != nullptr);
	return (BlockC<T, A>*)data(row);
}

template <class T, unsigned int A>
const ir::BlockC<T, A> * restrict ir::MatrixC<T, A>::block_data(unsigned int row) const noexcept
{
	assert(_data != nullptr);
	return (const BlockC<T, A>*)data(row);
}

template <class T, unsigned int A>
inline ir::BlockC<T, A> & restrict ir::MatrixC<T, A>::block(unsigned int row, unsigned int columnblock) noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	assert(columnblock < block_width());
	return block_data(row)[columnblock];
}

template <class T, unsigned int A>
inline const ir::BlockC<T, A> & restrict ir::MatrixC<T, A>::block(unsigned int row, unsigned int columnblock) const noexcept
{
	assert(_data != nullptr);
	assert(row < _height);
	assert(columnblock < block_width());
	return block_data(row)[columnblock];
}

template <class T, unsigned int A>
inline unsigned int ir::MatrixC<T, A>::complete_column_block_height() const noexcept
{
	assert(_data != nullptr);
	return _height / A;
}

template <class T, unsigned int A>
inline ir::BlockC<T, A> ir::MatrixC<T, A>::complete_column_block(unsigned int rowblock, unsigned int column) const noexcept
{
	assert(_data != nullptr);
	assert(rowblock < complete_column_block_height());
	assert(column < _width);
	ir::BlockC<T, A> b;
	for (unsigned int a = 0; a < A; a++) b.r[a] = at(A * rowblock + a, column);
	return b;
}

template <class T, unsigned int A>
inline ir::BlockC<T, A> ir::MatrixC<T, A>::column_block(unsigned int rowblock, unsigned int column) const noexcept
{
	assert(_data != nullptr);
	assert(rowblock < (_height + A - 1) / A);
	assert(column < _width);

	ir::BlockC<T, A> b;
	if (rowblock < complete_column_block_height())
	{
		return complete_column_block(rowblock, column);
	}
	else
	{
		b.assign_zero();
		for (unsigned int a = 0; a < _height - A * rowblock; a++) b.r[a] = at(A * rowblock + a, column);
		return b;
	}
}

template <class T, unsigned int A>
void ir::MatrixC<T, A>::assign_zero() noexcept
{
	assert(_data != nullptr);
	for (unsigned int i = 0; i < _height; i++)
	{
		for (unsigned int j = 0; j < _width; j++)
		{
			at(i, j) = 0;
		}
	}
}

template <class T, unsigned int A>
void ir::MatrixC<T, A>::assign_random(T low, T high) noexcept
{
	assert(_data != nullptr);
	std::default_random_engine engine;
	std::uniform_real_distribution<T> distribution(low, high);
	for (unsigned int i = 0; i < _height; i++)
	{
		for (unsigned int j = 0; j < _width; j++)
		{
			at(i, j) = distribution(engine);
		}
	}
}

template <class T, unsigned int A>
ir::MatrixC<T, A>::~MatrixC() noexcept
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
}

#ifdef restrict
	#undef restrict
#endif

#endif	//#ifndef IR_MATHC_IMPLEMENTATION