/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MATHC_IMPLEMENTATION
#define IR_MATHC_IMPLEMENTATION

#include <assert.h>

#ifdef IR_MATHC_3DNOW
	#include <type_traits>
	#ifdef _WIN32
		#include <intrin.h>
	#else
		#include <x86intrin.h>
	#endif	
#endif

template <typename T, unsigned int A>
ir::VectorC<T, A>::VectorC(unsigned int height, bool *ok)
{
	_height = height;
	unsigned int size = (block_height() * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr) memset(_data, 0, size);
	if (ok != nullptr) *ok = (_data != nullptr);
};

template <typename T, unsigned int A>
unsigned int ir::VectorC<T, A>::height() const
{
	return _height;
};

template <typename T, unsigned int A>
T * IR_MATHC_RESTRICT ir::VectorC<T, A>::data()
{
	return (_data != nullptr) ? (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1))) : nullptr;
};

template <typename T, unsigned int A>
const T * IR_MATHC_RESTRICT ir::VectorC<T, A>::data() const
{
	return (_data != nullptr) ? (const T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1))) : nullptr;
};

template <typename T, unsigned int A>
unsigned int ir::VectorC<T, A>::block_height() const
{
	return (_height + A - 1) / A;
};

template <typename T, unsigned int A>
ir::BlockC<T, A> * IR_MATHC_RESTRICT ir::VectorC<T, A>::block_data()
{
	return (BlockC<T, A>*)data();
};

template <typename T, unsigned int A>
const ir::BlockC<T, A> * IR_MATHC_RESTRICT ir::VectorC<T, A>::block_data() const
{
	return (BlockC<T, A>*)data();
};

template <typename T, unsigned int A>
ir::VectorC<T, A>::~VectorC()
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
};

template <typename T, unsigned int A>
ir::MatrixC<T, A>::MatrixC(unsigned int width, unsigned int height, bool *ok)
{
	_height = height;
	_width = width;
	unsigned int size = (height * block_width() * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr) memset(_data, 0, size);
	if (ok != nullptr) *ok = (_data != nullptr);
};

template <typename T, unsigned int A>
unsigned int ir::MatrixC<T, A>::width() const
{
	return _width;
};

template <typename T, unsigned int A>
unsigned int ir::MatrixC<T, A>::height() const
{
	return _height;
};

template <typename T, unsigned int A>
T * IR_MATHC_RESTRICT ir::MatrixC<T, A>::data(unsigned int line)
{
	if (_data == nullptr) return nullptr;
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + line * block_width() * A;
};

template <typename T, unsigned int A>
const T * IR_MATHC_RESTRICT ir::MatrixC<T, A>::data(unsigned int line) const
{
	if (_data == nullptr) return nullptr;
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + line * block_width() * A;
};

template <typename T, unsigned int A>
unsigned int ir::MatrixC<T, A>::block_width() const
{
	return (_width + A - 1) / A;
};

template <typename T, unsigned int A>
ir::BlockC<T, A> * IR_MATHC_RESTRICT  ir::MatrixC<T, A>::block_data(unsigned int line)
{
	return (BlockC<T, A>*)data(line);
};

template <typename T, unsigned int A>
const ir::BlockC<T, A> * IR_MATHC_RESTRICT ir::MatrixC<T, A>::block_data(unsigned int line) const
{
	return (const BlockC<T, A>*)data(line);
};

template <typename T, unsigned int A>
ir::MatrixC<T, A>::~MatrixC()
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
};

template<typename T, unsigned int A>
void ir::MathC<T, A>::add_vvv(const VectorC<T, A> *a, const VectorC<T, A> *b, VectorC<T, A> *r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->height() == b->height() && b->height() == r->height());
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b, r)
	#endif
	for (int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		for (unsigned int p = 0; p < A; p++)
			r->block_data()[lineblock].r[p] = a->block_data()[lineblock].r[p] + b->block_data()[lineblock].r[p];
	}
};

template<typename T, unsigned int A>
void ir::MathC<T, A>::subtract_vvv(const VectorC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->height() == b->height() && b->height() == r->height());
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b, r)
	#endif
	for (int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		for (unsigned int p = 0; p < A; p++)
			r->block_data()[lineblock].r[p] = a->block_data()[lineblock].r[p] - b->block_data()[lineblock].r[p];
	}
};

template<typename T, unsigned int A>
void ir::MathC<T, A>::multiply_mvv(const MatrixC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->width() == b->height() && a->height() == r->height());
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, b, r)
	#endif
	for (int line = 0; (unsigned int)line < r->height(); line++)
	{
		BlockC<T, A> sum;
		for (unsigned int p = 0; p < A; p++) sum.r[p] = 0.0;
		for (unsigned int columnblock = 0; columnblock < a->block_height(); columnblock++)
		{
			for (unsigned int p = 0; p < A; p++)
				sum.r[p] += a->block_data(line)[columnblock].r[p] * b->block_data()[columnblock].r[p];
		}
		T linesum = 0.0;
		for (unsigned int p = 0; p < A; p++) linesum += sum.r[p];
		r->data()[line] = linesum;
	}
};

#ifdef IR_MATHC_3DNOW

template<>
void ir::MathC<float, 2>::add_vvv(const VectorC<float, 2> *a, const VectorC<float, 2> *b, VectorC<float, 2> *r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->height() == b->height() && b->height() == r->height());
	
	const __m64 * IR_MATHC_RESTRICT adata = (const __m64*)a->block_data();
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	__m64 * IR_MATHC_RESTRICT rdata = (__m64*)r->block_data();
	
	for (unsigned int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		rdata[lineblock] = _m_pfadd(adata[lineblock], bdata[lineblock]);
	}
	_m_femms();
};

template<>
void ir::MathC<float, 2>::subtract_vvv(const VectorC<float, 2> * IR_MATHC_RESTRICT a, const VectorC<float, 2> * IR_MATHC_RESTRICT b, VectorC<float, 2> * IR_MATHC_RESTRICT r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->height() == b->height() && b->height() == r->height());

	const __m64 * IR_MATHC_RESTRICT adata = (const __m64*)a->block_data();
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	__m64 * IR_MATHC_RESTRICT rdata = (__m64*)r->block_data();

	for (unsigned int lineblock = 0; lineblock < a->block_height(); lineblock++)
	{
		rdata[lineblock] = _m_pfsub(adata[lineblock], bdata[lineblock]);
	}
	_m_femms();
};

template<>
void ir::MathC<float, 2>::multiply_mvv(const MatrixC<float, 2> * IR_MATHC_RESTRICT a, const VectorC<float, 2> * IR_MATHC_RESTRICT b, VectorC<float, 2> * IR_MATHC_RESTRICT r)
{
	assert(a != nullptr && b != nullptr && r != nullptr);
	assert(a != b && b != r && a != r);
	assert(a->width() == b->height() && a->height() == r->height());
	
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	const __m64 zero = _m_from_int(0);

	for (unsigned int line = 0; line < r->height(); line++)
	{
		const __m64 * IR_MATHC_RESTRICT alinedata = (const __m64*)a->block_data(line);
		__m64 sum = zero;
		for (unsigned int columnblock = 0; columnblock < a->block_width(); columnblock++)
		{
			sum = _m_pfadd(sum, _m_pfmul(alinedata[columnblock], bdata[columnblock]));
		}
		sum = _m_pfacc(sum, zero);
		*((int*)(r->data() + line)) = _m_to_int(sum);
	}
	_m_femms();
	return true;
};

#endif	//#ifdef IR_MATHC_3DNOW

#endif	//#ifndef IR_MATHC_IMPLEMENTATION