/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MATHC
#define IR_MATHC

#ifndef restrict
	#ifdef __cplusplus
		#define restrict __restrict
	#endif
#endif

namespace ir
{
///@defgroup MathC
///@{
	
	///Block of @c A numbers of type @c T, basic unit that MathC operates with
	template<class T, unsigned int A> struct alignas(A * sizeof(T)) BlockC
	{
		T r[A];
		inline void assign_zero() noexcept;
		inline T sum() const noexcept;
		inline void operator+=(const BlockC & restrict another) noexcept;
		inline void operator-=(const BlockC & restrict another) noexcept;
		inline void operator*=(const BlockC & restrict another) noexcept;
		inline void operator/=(const BlockC & restrict another) noexcept;
		inline BlockC operator+(const BlockC & restrict another) const noexcept;
		inline BlockC operator-(const BlockC & restrict another) const noexcept;
		inline BlockC operator*(const BlockC & restrict another) const noexcept;
		inline BlockC operator/(const BlockC & restrict another) const noexcept;
	};
	
	template <class T, unsigned int A> class MatrixC;

	///Vector, one-dimensional array of type @c T
	template <class T, unsigned int A> class VectorC
	{
		friend MatrixC<T, A>;

	private:
		void * restrict _data = nullptr;
		unsigned int _height = 0;
		VectorC(VectorC &vector);
		
	public:
		///Creates vector and allocates memory for it
		///@param height Dimension (height) of the vector
		///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
		VectorC(unsigned int height, bool *ok) noexcept;
		
		//Element access:
		///Returns dimension (height) of vector
		inline unsigned int height() const noexcept;
		///Returns dimension (height) of vector
		inline T * restrict data() noexcept;
		///Returns constant pointer to data
		inline const T * restrict data() const noexcept;
		///Returns reference to element of vector
		inline T & restrict at(unsigned int row) noexcept;
		///Returns constant reference to element of vector
		inline const T & restrict at(unsigned int row) const noexcept;
		
		//Block access:
		///Returns dimension (height) of vector in blocks
		inline unsigned int block_height() const noexcept;
		///Returns pointer to data in blocks
		inline BlockC<T, A> * restrict block_data() noexcept;
		///Returns constant pointer to data in blocks
		inline const BlockC<T, A> * restrict block_data() const noexcept;
		///Returns reference to block of vector
		inline BlockC<T, A> & restrict block(unsigned int rowblock) noexcept;
		///Returns constant reference to block of vector
		inline const BlockC<T, A> & restrict block(unsigned int rowblock) const noexcept;

		//Operations:
		///Assigns vector to zero values
		void assign_zero();
		///Assigns vector to random values
		void assign_random(T low, T high);
		///Assigns vector to vector sum
		void assign_add(const VectorC * restrict a, const VectorC * restrict b) noexcept;
		///Assigns vector to vector difference
		void assign_sub(const VectorC * restrict a, const VectorC * restrict b) noexcept;
		///Assigns vector to matix-vector product
		void assign_mul(const MatrixC<T, A> * restrict a, const VectorC * restrict b) noexcept;

		///Destroys vector
		~VectorC() noexcept;
	};

	///Matrix, two-dimensional array of type @c T
	template <class T, unsigned int A> class MatrixC
	{
		friend VectorC<T, A>;

	private:
		void *_data				= nullptr;
		unsigned int _width		= 0;
		unsigned int _height	= 0;
		MatrixC(MatrixC &matrix);

	public:
		///Creates matrix and allocates memory for it
		///@param width Width of the matrix
		///@param height Height of the matrix
		///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
		MatrixC(unsigned int width, unsigned int height, bool *ok) noexcept;

		//Element access:
		///Returns width of matrix
		inline unsigned int width() const noexcept;
		///Returns height of matrix
		inline unsigned int height() const noexcept;
		///Returns pointer to data on specified row
		inline T * restrict data(unsigned int row) noexcept;
		///Returns constant pointer to data on specified row
		inline const T * restrict data(unsigned int row) const noexcept;
		///Returns reference to element of matrix
		inline T & restrict at(unsigned int row, unsigned int column) noexcept;
		///Returns constant reference to element of matrix
		inline const T & restrict at(unsigned int row, unsigned int column) const noexcept;

		//Block access:
		///Returns width of matrix in blocks
		unsigned int block_width() const noexcept;
		///Returns pointer to data on specified row in blocks
		inline BlockC<T, A> * restrict block_data(unsigned int row) noexcept;
		///Returns constant pointer to data on specified row in blocks
		inline const BlockC<T, A> * restrict block_data(unsigned int row) const noexcept;
		///Returns reference to block of vector
		inline BlockC<T, A> & restrict block(unsigned int row, unsigned int columnblock) noexcept;
		///Returns constant reference to block of vector
		inline const BlockC<T, A> & restrict block(unsigned int row, unsigned int columnblock) const noexcept;
		///Returns number of column-major blocks can be safely read with @c main_column_block
		inline unsigned int complete_column_block_height() const noexcept;
		///Returns column-major block, may be faster than @c column_block
		inline BlockC<T, A> complete_column_block(unsigned int rowblock, unsigned int column) const noexcept;
		///Returns column-major block
		inline BlockC<T, A> column_block(unsigned int rowblock, unsigned int column) const noexcept;
		
		//Operations:
		///Assigns matrix to zero values
		void assign_zero() noexcept;
		///Assigns matrix to random values
		void assign_random(T low, T high) noexcept;

		///Destroys matrix
		~MatrixC() noexcept;
	};
	
///@}
}

#ifdef restrict
	#undef restrict
#endif

#if (defined(IR_IMPLEMENT) || defined(IR_MATHC_IMPLEMENT)) && !defined(IR_MATHC_NOT_IMPLEMENT)
	#include <implementation/ir_math/ir_mathc_implementation.h>
#endif

#endif	//#ifndef IR_MATHC