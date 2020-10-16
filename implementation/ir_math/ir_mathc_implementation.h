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

#ifdef IR_MATHC_3DNOW
	#include <type_traits>
	#ifdef _WIN32
		#include <intrin.h>
	#else
		#include <x86intrin.h>
	#endif	
#endif

template <typename T, unsigned int A> ir::VectorC<T, A>::VectorC(unsigned int height, bool *ok)
{
	_height = height;
	unsigned int size = (block_height() * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr) memset(_data, 0, size);
	if (ok != nullptr) *ok = (_data != nullptr);
};

template <typename T, unsigned int A> unsigned int ir::VectorC<T, A>::height() const
{
	return _heigth;
};

template <typename T, unsigned int A> T *ir::VectorC<T, A>::data()
{
	return (_data != nullptr) ? (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1))) : nullptr;
};

template <typename T, unsigned int A> const T *ir::VectorC<T, A>::data() const
{
	return (_data != nullptr) ? (const T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1))) : nullptr;
};

template <typename T, unsigned int A> unsigned int ir::VectorC<T, A>::block_height() const
{
	return (_height + A - 1) / A;
};

template <typename T, unsigned int A> ir::BlockC<T, A> *ir::VectorC<T, A>::block_data()
{
	return (BlockC<T, A>*)data();
};

template <typename T, unsigned int A> const ir::BlockC<T, A> *ir::VectorC<T, A>::block_data() const
{
	return (BlockC<T, A>*)data();
};

template <typename T, unsigned int A> ir::VectorC<T, A>::~VectorC()
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
};

template <typename T, unsigned int A> ir::MatrixC<T, A>::MatrixC(unsigned int width, unsigned int height, bool *ok)
{
	_height = height;
	_width = width;
	unsigned int size = (height * block_width() * A + A - 1) * sizeof(T);
	_data = malloc(size);
	if (_data != nullptr) memset(_data, 0, size);
	if (ok != nullptr) *ok = (_data != nullptr);
};

template <typename T, unsigned int A> unsigned int ir::MatrixC<T, A>::width() const
{
	return _width;
};

template <typename T, unsigned int A> unsigned int ir::MatrixC<T, A>::height() const
{
	return _height;
};

template <typename T, unsigned int A> T *ir::MatrixC<T, A>::data(unsigned int line)
{
	if (_data == nullptr) return nullptr;
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + line * block_width() * A;
};

template <typename T, unsigned int A> const T *ir::MatrixC<T, A>::data(unsigned int line) const
{
	if (_data == nullptr) return nullptr;
	T *aligneddata = (T*)(((size_t)_data + A * sizeof(T) - 1) & (~(A * sizeof(T) - 1)));
	return aligneddata + line * block_width() * A;
};

template <typename T, unsigned int A> unsigned int ir::MatrixC<T, A>::block_width() const
{
	return (_width + A - 1) / A;
};

template <typename T, unsigned int A> ir::BlockC<T, A> *ir::MatrixC<T, A>::block_data(unsigned int line)
{
	return (BlockC<T, A>*)data(line);
};

template <typename T, unsigned int A> const ir::BlockC<T, A> *ir::MatrixC<T, A>::block_data(unsigned int line) const
{
	return (const BlockC<T, A>*)data(line);
};

template <typename T, unsigned int A> ir::MatrixC<T, A>::~MatrixC()
{
	if (_data != nullptr) free(_data);
	_data = nullptr;
};

template<typename T, unsigned int A> bool ir::MathC<T, A>::add_vvv(const VectorC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| a == b || a == r || b == r
	|| a->_height != b->_height || r->_height != b->_height) return false;
	
	unsigned int blockheight = a->block_height();
	const BlockC<T, A> * IR_MATHC_RESTRICT adata = a->block_data();
	const BlockC<T, A> * IR_MATHC_RESTRICT bdata = b->block_data();
	BlockC<T, A> * IR_MATHC_RESTRICT rdata = r->block_data();
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(blockheight, adata, bdata, rdata)
	#endif
	for (int blockline = 0; blockline < blockheight; blockline++)
	{
		for (unsigned int p = 0; p < A; p++) rdata[blockline].r[p] = adata[blockline].r[p] + bdata[blockline].r[p];
	}
	return true;
};

template<typename T, unsigned int A> bool ir::MathC<T, A>::subtract_vvv(const VectorC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| a == b || a == r || b == r
	|| a->_height != b->_height || r->_height != b->_height) return false;
	
	unsigned int blockheight = a->block_height();
	const BlockC<T, A> * IR_MATHC_RESTRICT adata = a->block_data();
	const BlockC<T, A> * IR_MATHC_RESTRICT bdata = b->block_data();
	BlockC<T, A> * IR_MATHC_RESTRICT rdata = r->block_data();
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(blockheight, adata, bdata, rdata)
	#endif
	for (int blockline = 0; blockline < blockheight; blockline++)
	{
		for (unsigned int p = 0; p < A; p++) rdata[blockline].r[p] = adata[blockline].r[p] - bdata[blockline].r[p];
	}
	return true;
};

template<typename T, unsigned int A> bool ir::MathC<T, A>::multiply_mvv(const MatrixC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| b == r
	|| a->_width != b->_height || a->_height != r->_height) return false;
	
	unsigned int height = a->_height;
	unsigned int blockwidth = a->block_width();
	const BlockC<T, A> * IR_MATHC_RESTRICT bdata = b->block_data();
	T * IR_MATHC_RESTRICT rdata = r->data();
	
	#ifdef IR_MATHC_OPENMP
		#pragma omp parallel for firstprivate(a, height, blockwidth, bdata, rdata)
	#endif
	for (int line = 0; (unsigned int)line < height; line++)
	{
		const BlockC<T, A> * IR_MATHC_RESTRICT alinedata = a->block_data(line);
		BlockC<T, A> sum;
		for (unsigned int p = 0; p < A; p++) sum.r[p] = 0.0f;
		for (unsigned int columnblock = 0; columnblock < blockwidth; columnblock++)
		{
			for (unsigned int p = 0; p < A; p++) sum.r[p] += alinedata[columnblock].r[p] * bdata[columnblock].r[p];
		}
		float endsum = 0.0f;
		for (unsigned int p = 0; p < A; p++) endsum += sum.r[p];
		rdata[line] = endsum;
	}
	return true;
};

#ifdef IR_MATHC_3DNOW

template<> bool ir::MathC<float, 2>::add_vvv(const VectorC<float, 2> * IR_MATHC_RESTRICT a, const VectorC<float, 2> * IR_MATHC_RESTRICT b, VectorC<float, 2> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| a == b || a == r || b == r
	|| a->_height != b->_height || r->_height != b->_height) return false;
	
	unsigned int blockheight = a->block_height();
	const __m64 * IR_MATHC_RESTRICT adata = (const __m64*)a->block_data();
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	__m64 * IR_MATHC_RESTRICT rdata = (__m64*)r->block_data();
	
	for (unsigned int blockline = 0; blockline < blockheight; blockline++)
	{
		rdata[blockline] = _m_pfadd(adata[blockline], bdata[blockline]);
	}
	_m_femms();
	return true;
};

template<> bool ir::MathC<float, 2>::subtract_vvv(const VectorC<float, 2> * IR_MATHC_RESTRICT a, const VectorC<float, 2> * IR_MATHC_RESTRICT b, VectorC<float, 2> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| a == b || a == r || b == r
	|| a->_height != b->_height || r->_height != b->_height) return false;
	
	unsigned int blockheight = a->block_height();
	const __m64 * IR_MATHC_RESTRICT adata = (const __m64*)a->block_data();
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	__m64 * IR_MATHC_RESTRICT rdata = (__m64*)r->block_data();
	
	for (unsigned int blockline = 0; blockline < blockheight; blockline++)
	{
		rdata[blockline] = _m_pfsub(adata[blockline], bdata[blockline]);
	}
	_m_femms();
	return true;
};

template<> bool ir::MathC<float, 2>::multiply_mvv(const MatrixC<float, 2> * IR_MATHC_RESTRICT a, const VectorC<float, 2> * IR_MATHC_RESTRICT b, VectorC<float, 2> * IR_MATHC_RESTRICT r)
{
	if(a->_data == nullptr || b->_data == nullptr || r->_data == nullptr
	|| b == r
	|| a->_width != b->_height || a->_height != r->_height) return false;
	
	unsigned int height = a->_height;
	unsigned int blockwidth = a->block_width();
	const __m64 * IR_MATHC_RESTRICT bdata = (const __m64*)b->block_data();
	int * IR_MATHC_RESTRICT rdata = (int*)r->data();
	
	const __m64 zero = _m_from_int(0);
	for (unsigned int line = 0; line < height; line++)
	{
		const __m64 * IR_MATHC_RESTRICT alinedata = (const __m64*)a->block_data(line);
		__m64 sum = zero;
		for (unsigned int columnblock = 0; columnblock < blockwidth; columnblock++)
		{
			sum = _m_pfadd(sum, _m_pfmul(alinedata[columnblock], bdata[columnblock]));
		}
		sum = _m_pfacc(sum, zero);
		rdata[line] = _m_to_int(sum);
	}
	_m_femms();
	return true;
};

#endif	//#ifdef IR_MATHC_3DNOW

#endif	//#ifndef IR_MATHC_IMPLEMENTATION