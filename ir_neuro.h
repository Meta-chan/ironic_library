/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ATTENTION!
//For optimization purpose along with standart IR_NEURO_[NOT]_IMPLEMENT macro
//this header provides:
//IR_NEURO_CRITICAL_[NOT]_IMPLEMENT - to manage whether to include time critical code or not.
//IR_NEURO_CRITICAL_NOT_RESTRICT - to disable restrict modifier. May reduce performance.
//IR_NEURO_CRITICAL_OPENMP - to enable OpenMP.
//IR_NEURO_CRITICAL_3DNOW - to use 3DNow! instructions if Align is 2. Use if you are freak.

#ifndef IR_NEURO
#define IR_NEURO

#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <stdio.h>
#include <stddef.h>

#ifdef IR_NEURO_CRITICAL_NOT_RESTRICT
	#define IR_NEURO_CRITICAL_RESTRICT
#else
	#define IR_NEURO_CRITICAL_RESTRICT __restrict
#endif

#define _IR_NEURO_UPALIGN(_ALIGN, _VALUE) ((((size_t)_VALUE) + _ALIGN - 1) & ~(_ALIGN - 1))
#define _IR_NEURO_DOWNALIGN(_ALIGN, _VALUE) (((size_t)_VALUE) & ~(_ALIGN - 1))
#define _IR_NEURO_BLOCK_UPALIGN(_VALUE) ((FloatBlock*)((((size_t)_VALUE) + sizeof(FloatBlock) - 1) & ~(sizeof(FloatBlock) - 1)))
#define _IR_NEURO_BLOCK_DOWNALIGN(_VALUE) ((FloatBlock*)(((size_t)_VALUE) & ~(sizeof(FloatBlock) - 1)))

namespace ir
{	
///@defgroup ir_neuro Neuronal network
///@{

	///Hyperbolic tangens function, may be passed as ir::Neuro template parameter
	class TanhFunction
	{
	public:
		static inline float function(const float input);	///< Hyperbolic tangens
		static inline float derivative(const float output);	///< Derivative of hyperbolic tangens calculated from it's result
	};
	
	///ReLU function, may be passed as ir::Neuro template parameter
	class ReLUFunction
	{
	public:
		static inline float function(const float input);	///< ReLU fucntion (ReLU(x) = x if x >= 0, ReLU(x) = 0.01 * x if x < 0)
		static inline float derivative(const float output);	///< Derivative of ReLU calculated from it's result
	};

	///Ultra-lite neural network with teacher, provides no GPU acceleration
	///@tparam ActivationFunction Activation function of the network
	///@tparam Align Alignment of input, output, goal and internal buffers in floats, used for SIMD acceleration. If Align == 2, the network may be accelerated with 3DNow! instructions
	///@remarks
	///ir::Neuro tries to skip copy operations. It's internal buffers are properly aligned, they also have one extra adjustment input that is always 1.0f, and after it and till end of alignment block inputs are set to 0.0f. If you have a buffer that corresponds these conditions, ir::Neuro will not copy it to internal buffer, which saves time. This is the default option. @n But if you do not want to care about it, set @c copy parameter to true. ir::Neuro will take care of it.
	template <class ActivationFunction = TanhFunction, unsigned int Align = 1> class Neuro
	{
	private:
		struct FileHeader
		{
			char signature[3]		= { 'I', 'N', 'R' };
			unsigned char version	= 2;
		};

		struct alignas(Align * sizeof(float)) FloatBlock
		{
			float f[Align];
		};

		bool _ok				= false;
		unsigned int _nlayers	= 0;
		unsigned int *_layers	= nullptr;
		float **_weights		= nullptr;
		float **_vectors		= nullptr;
		float **_errors			= nullptr;
		float **_prev_changes	= nullptr;
		float *_goal			= nullptr;
		float _coefficient		= 0.0f;
		float _inductance		= 0.0f;

		const float *_user_input= nullptr;
		const float *_user_goal	= nullptr;
		float *_user_output		= nullptr;
		bool _copy_output		= false;
		
		ec _init_matrixes(float ***matrixes, float amplitude, FILE *file);
		ec _init(float amplitude, FILE *file);
		ec _init_from_random(unsigned int nlayers, const unsigned int *layers, float amplitude);
		ec _init_from_file(const syschar *filepath);
		ec _save_matrixes(float*const*const matrixes, FILE *file) const;
		static void _free_vectors(float **vector, unsigned int n);

		static void _step_forward(
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix,
			unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
			unsigned int nextlen, float *IR_NEURO_CRITICAL_RESTRICT nextvector);
		static void _last_backward(
			unsigned int lastlen,
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT goal,
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT lastoutput,
			FloatBlock *IR_NEURO_CRITICAL_RESTRICT lasterror);
		static void _step_backward(
			const float *IR_NEURO_CRITICAL_RESTRICT matrix,
			unsigned int nextlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT nexterror,
			unsigned int prevlen, const float *IR_NEURO_CRITICAL_RESTRICT prevvector, float *IR_NEURO_CRITICAL_RESTRICT preverror);
		static void _corrigate(
			float coefficient, float inductance,
			unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
			unsigned int nextlen, const float *IR_NEURO_CRITICAL_RESTRICT nexterror,
			FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix, FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevchanges);

	public:
		///Creates the network based on number of neurons in each layer, where 0 is input layer and <tt>nlayers - 1</tt> is output layer. Initializes weights with random values from <tt>-amplitude</tt> to <tt>amplitude</tt>. If code is not @c nullptr, receives return code
		Neuro(unsigned int nlayers, const unsigned int *layers, float amplitude, ec *code);
		///Loads the network from file
		Neuro(const syschar *filepath, ec *code);
		///Sets the input of the net. See remarks.
		ec set_input(const float *input, bool copy = false);
		///Sets the goal i.e. wished result of the forward function. See remarks.
		ec set_goal(const float *goal, bool copy = false);
		///Sets the learning coefficient
		ec set_coefficient(float coefficient);
		///Sets the inductance coefficient. Great inductance coefficient causes the network to change slower, but allows it to leave local minimums
		ec set_inductance(float inductance);
		///Sets a pointer to output array. See remarks.
		ec set_output_pointer(float *output, bool copy = false);
		///Forces the network to put output value of forward into array specified by set_output_pointer
		ec get_output() const;
		///Perform forward calculation
		ec forward();
		///Perform backward learning. Needs to be called after forward
		ec backward();
		///Save the network to file
		ec save(const syschar *filepath) const;
		///Destroys the network
		~Neuro();
	};
	
///@}
};

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_IMPLEMENT)) && !defined(IR_NEURO_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_implementation.h>
#endif

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_CRITICAL_IMPLEMENT)) && !defined(IR_NEURO_CRITICAL_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_critical_implementation.h>
#endif

#endif	//#ifndef IR_NEURO