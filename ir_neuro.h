/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ATTENTION!
//For optimization purpose along with standart IR_NEURO_[NOT]_IMPLEMENT macro
//this header provides:
//IR_NEURO_CRITICAL_[NOT]_IMPLEMENT - to manage whether to include time critical code or not.
//IR_NEURO_CRITICAL_RESTRICT - to use own restriction modifier. If not defined, restrict for C and __restrict for C++
//IR_NEURO_CRITICAL_OPENMP - to enable OpenMP.

#ifndef IR_NEURO
#define IR_NEURO

#include <ir_math/ir_mathc.h>
#include <ir_container/ir_quiet_vector.h>
#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <stdio.h>
#include <stddef.h>

#ifndef IR_NEURO_CRITICAL_RESTRICT
	#ifdef __cplusplus
		#define IR_NEURO_CRITICAL_RESTRICT __restrict
	#else
		#define IR_NEURO_CRITICAL_RESTRICT restrict
	#endif
#endif

namespace ir
{	
///@defgroup ir_neuro Neuronal network
///@{

	///Hyperbolic tangens function, may be passed as ir::Neuro template parameter
	template<class T> class Tanh
	{
	public:
		static inline T function(const T input)		noexcept;	///< Hyperbolic tangens
		static inline T derivative(const T output)	noexcept;	///< Derivative of hyperbolic tangens calculated from it's result
	};
	
	///ReLU function, may be passed as ir::Neuro template parameter
	template<class T> class ReLU
	{
	public:
		static inline T function(const T input)		noexcept;	///< ReLU fucntion (ReLU(x) = x if x >= 0, ReLU(x) = 0.01 * x if x < 0)
		static inline T derivative(const T output)	noexcept;	///< Derivative of ReLU calculated from it's result
	};

	///Ultra-lite neural network with teacher, provides no GPU acceleration
	///@tparam T Type of numbers the network operats with
	///@tparam A Alignment of input, output, goal and internal buffers in T's, used for SIMD acceleration
	///@tparam F Activation function of the network
	///@remark
	///ir::Neuro tries to skip copy operations. It's internal buffers are properly aligned, they also have one extra adjustment input that is always 1.0f, and after it and till end of alignment block inputs are set to 0.0f. If you have a buffer that corresponds these conditions, ir::Neuro will not copy it to internal buffer, which saves time. This is the default option. @n But if you do not want to care about it, set @c copy parameter to true. ir::Neuro will take care of it.
	template <class T = float, unsigned int A = 1, class F = Tanh<T>> class Neuro
	{
	private:
		struct FileHeader
		{
			char signature[3]		= { 'I', 'N', 'R' };
			unsigned char version	= 3;
		};

		bool _ok = false;
		QuietVector<unsigned int> _layers;
		QuietVector<VectorC<T, A>> _vectors;
		QuietVector<VectorC<T, A>> _errors;
		VectorC<T, A> _goal;
		QuietVector<MatrixC<T, A>> _weights; 
		T _coefficient = 0.0;
		
		static void _forward(const MatrixC<T, A> *w, const VectorC<T, A> *pv, VectorC<T, A> *nv) noexcept;
		static void _lastbackward(const VectorC<T, A> *g, const VectorC<T, A> *l, VectorC<T, A> *e) noexcept;
		static void _backward(const MatrixC<T, A> *w, const VectorC<T, A> *ne, const VectorC<T, A> *pv, VectorC<T, A> *pe) noexcept;
		static void _corrigate(T coef, const VectorC<T, A> *pv, const VectorC<T, A> *ne, MatrixC<T, A> *w) noexcept;
		ec _init(T amplitude, FILE *file) noexcept;

	public:
		///Creates the network based on number of neurons in each layer,
		///where 0 is input layer and <tt>nlayers - 1</tt> is output layer.
		///Initializes weights with random values from <tt>-amplitude</tt> to <tt>amplitude</tt>
		Neuro(unsigned int nlayers, const unsigned int *layers, T amplitude, ec *code) noexcept;
		///Loads the network from file
		Neuro(const syschar *filepath, ec *code) noexcept;
		///Returns if ir::Neuro was created properly
		bool ok() const noexcept;
		///Gets input
		VectorC<T, A> *get_input() noexcept;
		///Gets output
		VectorC<T, A> *get_output() noexcept;
		///Gets goal
		VectorC<T, A> *get_goal() noexcept;
		///Sets learning coefficient
		void set_coefficient(T coefficient) noexcept;
		///Performs forward calculation
		void forward() noexcept;
		///Performs backward learning. Needs to be called after forward
		void backward() noexcept;
		///Saves the network to file, does not modify error code
		ec save(const syschar *filepath) const noexcept;
		///Destroys the network
		~Neuro() noexcept;
	};
	
///@}
}

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_CRITICAL_IMPLEMENT)) && !defined(IR_NEURO_CRITICAL_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_critical_implementation.h>
#endif

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_IMPLEMENT)) && !defined(IR_NEURO_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_implementation.h>
#endif

#endif	//#ifndef IR_NEURO