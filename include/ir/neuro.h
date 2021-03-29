/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_NEURO
#define IR_NEURO

#include "matrix.h"
#include "ec.h"
#include "types.h"
#include <stdio.h>
#include <stddef.h>
#include <vector>

namespace ir
{	
///@addtogroup neuro Neuronal networks
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
		static inline T function(const T input)		noexcept;	///< ReLU function (ReLU(x) = x if x >= 0, ReLU(x) = 0.01 * x if x < 0)
		static inline T derivative(const T output)	noexcept;	///< Derivative of ReLU calculated from it's result
	};

	///Ultra-lite neural network with teacher, provides no GPU acceleration. Kind of CPU version of [NeuroG](https://github.com/Meta-chan/NeuroG)
	///@tparam T Type of numbers the network operates with
	///@tparam A Alignment of input, output, goal and internal buffers in T's, used for SIMD acceleration
	///@tparam F Activation function of the network
	template <class T = float, size_t A = 1, class F = Tanh<T>> class Neuro
	{
	private:
		struct FileHeader
		{
			char signature[3]		= { 'I', 'N', 'R' };
			unsigned char version	= 4;
		};

		bool _ok = false;
		std::vector<uint32> _layers;
		std::vector<Matrix<T, A>> _vectors;
		std::vector<Matrix<T, A>> _errors;
		Matrix<T, A> _goal;
		std::vector<Matrix<T, A>> _weights; 
		T _coefficient = 0.0;
		
		static void _forward(const Matrix<T, A> *w, const Matrix<T, A> *pv, Matrix<T, A> *nv)							noexcept;
		static void _lastbackward(const Matrix<T, A> *g, const Matrix<T, A> *l, Matrix<T, A> *e)						noexcept;
		static void _backward(const Matrix<T, A> *w, const Matrix<T, A> *ne, const Matrix<T, A> *pv, Matrix<T, A> *pe)	noexcept;
		static void _corrigate(T coef, const Matrix<T, A> *pv, const Matrix<T, A> *ne, Matrix<T, A> *w)					noexcept;
		ec _init(T amplitude, FILE *file)																				noexcept;

	public:
		///Creates the network based on number of neurons in each layer,
		///where 0 is input layer and `nlayers - 1` is output layer.
		///Initializes weights with random values from `-amplitude` to `amplitude`
		Neuro(size_t nlayers, const uint32 *layers, T amplitude, ec *code)	noexcept;
		///Loads the network from file
		Neuro(const schar *filepath, ec *code)								noexcept;
		///Returns if `ir::Neuro` was created properly
		bool ok()															const noexcept;
		///Gets input
		Matrix<T, A> *get_input()											noexcept;
		///Gets output
		Matrix<T, A> *get_output()											noexcept;
		///Gets goal
		Matrix<T, A> *get_goal()											noexcept;
		///Sets learning coefficient
		void set_coefficient(T coefficient)									noexcept;
		///Returns learning coefficient
		T get_coefficient()													const noexcept;
		///Performs forward calculation
		void forward()														noexcept;
		///Performs backward learning. Needs to be called after forward
		void backward()														noexcept;
		///Saves the network to file, does not modify error code
		ec save(const schar *filepath)										const noexcept;
		///Destroys the network
		~Neuro()															noexcept;
	};
	
///@}
}

#endif	//#ifndef IR_NEURO

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_NEURO) : !defined(IR_EXCLUDE_NEURO)
	#ifndef IR_INCLUDE
		#ifndef IR_NEURO_INLINE_SOURCE
			#define IR_NEURO_INLINE_SOURCE
			#include "../../source/inline/neuro.h"
		#endif
		#ifndef IR_NEURO_TEMPLATE_SOURCE
			#define IR_NEURO_TEMPLATE_SOURCE
			#include "../../source/template/neuro.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_NEURO_INLINE_SOURCE
			#define IR_NEURO_INLINE_SOURCE
			#include "../../source/inline/neuro.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_NEURO_INLINE_SOURCE
			#define IR_NEURO_INLINE_SOURCE
			#include "../../source/inline/neuro.h"
		#endif
		#ifndef IR_NEURO_TEMPLATE_SOURCE
			#define IR_NEURO_TEMPLATE_SOURCE
			#include "../../source/template/neuro.h"
		#endif
	#endif
#endif