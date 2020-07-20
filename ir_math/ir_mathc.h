/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MATHC
#define IR_MATHC

#ifdef IR_MATHC_NO_RESTRICT
	#define IR_MATHC_RESTRICT
#else
	#define IR_MATHC_RESTRICT __restrict
#endif

namespace ir
{
///@defgroup MathC
///@{
	
	template<typename T, unsigned int A> class MathC;

	///Block of @c A numbers of type @c T, basic unit that MathC operates with
	template<typename T, unsigned int A> struct alignas(A * sizeof(T)) BlockC
	{
		float r[A];
	};

	///Vector, one-dimensional array of type @c T
	template <typename T, unsigned int A> class VectorC
	{
		friend MathC<T, A>;

	private:
		void *_data				= nullptr;
		unsigned int _height	= 0;
		VectorC(VectorC &vector);
		
	public:
		///Creates vector and allocates memory for it
		///@param height Dimension (height) of the vector
		///@param ok If not @c nullptr, receives @c true or @c false dependent on success or fail
		VectorC(unsigned int height, bool *ok);
		///Returns dimension (height) of vector
		unsigned int height() const;
		///Returns dimension (height) of vector
		T *data();
		///Returns constant pointer to data
		const T *data() const;
		///Returns dimension (height) of vector in blocks
		unsigned int block_height() const;
		///Returns pointer to data in blocks
		BlockC<T, A> *block_data();
		///Returns constant pointer to data in blocks
		const BlockC<T, A> *block_data() const;
		///Destroys vector
		~VectorC();
	};

	///Matrix, two-dimensional array of type @c T
	template <typename T, unsigned int A> class MatrixC : public ObjectG
	{
		friend MathC<T, A>;

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
		MatrixC(unsigned int width, unsigned int height, bool *ok);
		///Returns width of matrix
		unsigned int width() const;
		///Returns height of matrix
		unsigned int height() const;
		///Returns pointer to data on specified line
		T *data(unsigned int line);
		///Returns constant pointer to data on specified line
		const T *data(unsigned int line) const;
		///Returns width of matrix in blocks
		unsigned int block_width() const;
		///Returns pointer to data on specified line in blocks
		BlockC<T, A> *block_data(unsigned int line);
		///Returns constant pointer to data on specified line in blocks
		const BlockC<T, A> *block_data(unsigned int line) const;
		///Destroys matrix
		~MatrixC();
	};

	///@brief MathC system, performs mathematical operations on <tt>ir::VectorC</tt> and <tt>ir::MatrixC</tt> using parallelism and vectorization.
	///@remark All vectors and matrices passed, both arguments and results, need to be @b different! Otherwise functions will fail.
	template<typename T, unsigned int A> class MathC
	{
	public:
		///Performs add operation on vectors <tt>r = a + b</tt>
		///@return @c true or @c false dependent on success or fail
		static bool add_vvv(const VectorC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r);
		///Performs subtract operation <tt>r = a - b</tt>
		///@return @c true or @c false dependent on success or fail
		static bool subtract_vvv(const VectorC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r);
		///Performs multiply operation <tt>r = A * b</tt>
		///@return @c true or @c false dependent on success or fail
		static bool multiply_mvv(const MatrixC<T, A> * IR_MATHC_RESTRICT a, const VectorC<T, A> * IR_MATHC_RESTRICT b, VectorC<T, A> * IR_MATHC_RESTRICT r);
	};
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_MATHC_IMPLEMENT)) && !defined(IR_MATHC_NOT_IMPLEMENT)
	#include <implementation/ir_math/ir_mathc_implementation.h>
#endif

#endif	//#ifndef IR_MATHC