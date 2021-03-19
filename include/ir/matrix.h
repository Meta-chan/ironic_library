/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MATRIX
#define IR_MATRIX

#ifndef IR_RESTRICT
	#define IR_RESTRICT __restrict
#endif

namespace ir
{
///@defgroup hiperf Hight performance computing
///@{
	
	///Group of `A` elements of type `T`@n
	///Desires to represent SIMD register and force compiler to use SIMD instructions.
	///@tparam T Basic type
	///@tparam A Alignment in basic elements. For example, with `T = float` and `A = 4` compiler will theoretically generate SSE code
	template <class T, size_t A> struct alignas(A * sizeof(T)) Chunk
	{
		T r[A];
		inline void zero()											noexcept;
		inline T sum()												const noexcept;
		inline void operator+=(const Chunk & IR_RESTRICT another)	noexcept;
		inline void operator-=(const Chunk & IR_RESTRICT another)	noexcept;
		inline void operator*=(const Chunk & IR_RESTRICT another)	noexcept;
		inline void operator/=(const Chunk & IR_RESTRICT another)	noexcept;
		inline Chunk operator+(const Chunk & IR_RESTRICT another)	const noexcept;
		inline Chunk operator-(const Chunk & IR_RESTRICT another)	const noexcept;
		inline Chunk operator*(const Chunk & IR_RESTRICT another)	const noexcept;
		inline Chunk operator/(const Chunk & IR_RESTRICT another)	const noexcept;
	};

	///Two-dimensional array of type `T` optimized for performance
	///@tparam T Basic type
	///@tparam A Alignment in basic elements. For example, with `T = float` and `A = 4` compiler will theoretically generate SSE code
	template <class T, size_t A> class Matrix
	{
	private:
		void *_data		= nullptr;
		size_t _width	= 0;
		size_t _height	= 0;

		Matrix(const Matrix &other) noexcept;

	public:
		///Creates empty matrix
		Matrix()								noexcept;
		///Creates matrix and allocates memory for it
		///@param width Width of the matrix
		///@param height Height of the matrix
		///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
		Matrix(size_t height, size_t width)		noexcept;
		///Initializes empty matrix
		bool init(size_t height, size_t width)	noexcept;
		///Checks if matrix is successfully allocated
		bool ok()								const noexcept;
		///Finalizes matrix and frees resources
		void finalize()							noexcept;
		///Destroys matrix
		~Matrix()								noexcept;

		//Element access:
		///Returns width of matrix
		inline size_t width()												const noexcept;
		///Returns height of matrix
		inline size_t height()												const noexcept;
		///Returns pointer to data on specified row
		inline T *IR_RESTRICT data(size_t row)								noexcept;
		///Returns constant pointer to data on specified row
		inline const T *IR_RESTRICT data(size_t row)						const noexcept;
		///Returns reference to element of matrix
		inline T & IR_RESTRICT at(size_t row, size_t column)				noexcept;
		///Returns constant reference to element of matrix
		inline const T & IR_RESTRICT at(size_t row, size_t column)			const noexcept;
		///Returns reference to element of matrix
		inline T & IR_RESTRICT operator()(size_t row, size_t column)		noexcept;
		///Returns constant reference to element of matrix
		inline const T & IR_RESTRICT operator()(size_t row, size_t column)	const noexcept;

		//Chunk access:
		///Returns pointer to data on specified row in chunks
		inline Chunk<T, A> *IR_RESTRICT chunk_data(size_t row)						noexcept;
		///Returns constant pointer to data on specified row in chunks
		inline const Chunk<T, A> *IR_RESTRICT chunk_data(size_t row)				const noexcept;
		///Returns reference to chunk of matrix
		inline Chunk<T, A> & IR_RESTRICT chunk_at(size_t row, size_t column)		noexcept;
		///Returns constant reference to chunk of matrix
		inline const Chunk<T, A> & IR_RESTRICT chunk_at(size_t row, size_t column)	const noexcept;
		///Returns vertical chunk of matrix
		inline Chunk<T, A> vertical_chunk_at(size_t row, size_t column)				const noexcept;
		///Returns vertical chunk of matrix at the edge of matrix
		inline Chunk<T, A> vertical_chunk_at_edge(size_t row, size_t column)		const noexcept;
		
		//Operations:
		///Assigns matrix to zero
		void zero()																				noexcept;
		///Assigns matrix to random value
		void random(T low, T high)																noexcept;
		///Adds matrix to given matrix
		void add(const Matrix *IR_RESTRICT b)													noexcept;
		///Subtracts matrix from given matrix
		void subtract(const Matrix *IR_RESTRICT b)												noexcept;
		///Multiplies matrix with given matrix element-wise
		void element_multiply(const Matrix *IR_RESTRICT b)										noexcept;
		///Assigns matrix to sum of matrixes
		void sum(const Matrix *IR_RESTRICT a, const Matrix *IR_RESTRICT b)						noexcept;
		///Assigns matrix to difference of matrixes
		void difference(const Matrix *IR_RESTRICT a, const Matrix *IR_RESTRICT b)				noexcept;
		///Assigns matrix to element-wise product of matrixes
		void element_product(const Matrix *IR_RESTRICT a, const Matrix *IR_RESTRICT b)			noexcept;
		///Assigns matrix to matrix product of matrix a and transposed matrix b
		void matrix_product_transposed(const Matrix *IR_RESTRICT a, const Matrix *IR_RESTRICT b)noexcept;
	};
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_MATRIX) : !defined(IR_EXCLUDE_MATRIX)
	#ifndef IR_INCLUDE
		#ifndef IR_MATRIX_INLINE_SOURCE
			#define IR_MATRIX_INLINE_SOURCE
			#include "../../source/inline/matrix.h"
		#endif
		#ifndef IR_MATRIX_TEMPLATE_SOURCE
			#define IR_MATRIX_TEMPLATE_SOURCE
			#include "../../source/template/matrix.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_MATRIX_INLINE_SOURCE
			#define IR_MATRIX_INLINE_SOURCE
			#include "../../source/inline/matrix.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_MATRIX_INLINE_SOURCE
			#define IR_MATRIX_INLINE_SOURCE
			#include "../../source/inline/matrix.h"
		#endif
		#ifndef IR_MATRIX_TEMPLATE_SOURCE
			#define IR_MATRIX_TEMPLATE_SOURCE
			#include "../../source/template/matrix.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_MATRIX