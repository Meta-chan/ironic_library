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
#include <stdlib.h>
#include <string.h>
#include <random>

template <class T, size_t A>
ir::Matrix<T, A>::Matrix() noexcept
{
}

template <class T, size_t A>
ir::Matrix<T, A>::Matrix(size_t height, size_t width) noexcept
{
	init(height, width);
}

template <class T, size_t A>
bool ir::Matrix<T, A>::init(size_t height, size_t width) noexcept
{
	finalize();
	size_t aligned_width = (width + A - 1) / A *A;
	size_t size = height *aligned_width *sizeof(T) + (A - 1) *sizeof(T);
	_data = malloc(size);
	if (_data != nullptr)
	{
		memset(_data, 0, size);
		_height = height;
		_width = width;
	}
	return _data != nullptr;
}

template <class T, size_t A>
bool ir::Matrix<T, A>::ok() const noexcept
{
	return _data != nullptr;
}

template <class T, size_t A>
void ir::Matrix<T, A>::finalize() noexcept
{
	if (_data != nullptr)
	{
		free(_data);
		_data = nullptr;
	}
	_height = 0;
	_width = 0;
}

template <class T, size_t A>
ir::Matrix<T, A>::~Matrix() noexcept
{
	finalize();
}

template<class T, size_t A>
void ir::Matrix<T, A>::zero() noexcept
{
	assert(_data != nullptr);
	size_t aligned_width = (width + A - 1) / A *A;
	size_t size = height *aligned_width *sizeof(T) + (A - 1) *sizeof(T);
	memset(_data, 0, size);
}

template <class T, size_t A>
void ir::Matrix<T, A>::random(T low, T high) noexcept
{
	assert(_data != nullptr);
	std::default_random_engine engine;
	std::uniform_real_distribution<T> distribution(low, high);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column++)
		{
			at(row, column) = distribution(engine);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::add(const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) += b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::subtract(const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) -= b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::element_multiply(const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) *= b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::sum(const Matrix<T, A> *IR_RESTRICT a, const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(a != nullptr && a != this && _height == a->_height && _width == a->_width);
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	assert(a != b);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) = a->chunk(row, column) + b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::difference(const Matrix<T, A> *IR_RESTRICT a, const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(a != nullptr && a != this && _height == a->_height && _width == a->_width);
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	assert(a != b);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) = a->chunk(row, column) + b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::element_product(const Matrix<T, A> *IR_RESTRICT a, const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(a != nullptr && a != this && _height == a->_height && _width == a->_width);
	assert(b != nullptr && b != this && _height == b->_height && _width == b->_width);
	assert(a != b);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column += A)
		{
			chunk_at(row, column) = a->chunk(row, column) * b->chunk(row, column);
		}
	}
}

template<class T, size_t A>
void ir::Matrix<T, A>::matrix_product_transposed(const Matrix<T, A> *IR_RESTRICT a, const Matrix<T, A> *IR_RESTRICT b) noexcept
{
	assert(a != nullptr && a != this);
	assert(b != nullptr && b != this);
	assert(a != b);
	assert(a->_width == b->_width);
	assert(a->_height == _height);
	assert(b->_height == _width);

	for (size_t row = 0; row < _height; row++)
	{
		for (size_t column = 0; column < _width; column++)
		{
			Chunk<T, A> sum;
			sum.zero();
			for (size_t i = 0; i < _width; i += A)
			{
				sum += a->chunk_at(row, i) * b->chunk_at(column, i);
			}
			at(row, column) = sum.sum();
		}
	}
}