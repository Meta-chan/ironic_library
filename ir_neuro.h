/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir::Neuro - simple single-core neuronal net

//ATTENTION!
//For optimization purpose this header provides not only IR_NEURO_[NOT]_IMPLEMENT interface
//It also does IR_NEURO_CRITICAL_[NOT]_IMPLEMENT for time critical code implementation
//IR_NEURO_CRITICAL_NOT_RESTRICT - to disable restrict modifier (may reduce performance)

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

#ifndef IR_NEURO_ALIGN
	#define IR_NEURO_ALIGN 1
#endif

#define _IR_NEURO_UPALIGN(_ALIGN, _VALUE) ((((size_t)_VALUE) + _ALIGN - 1) & ~(_ALIGN - 1))
#define _IR_NEURO_DOWNALIGN(_ALIGN, _VALUE) (((size_t)_VALUE) & ~(_ALIGN - 1))
#define _IR_NEURO_BLOCK_UPALIGN(_VALUE) ((FloatBlock*)((((size_t)_VALUE) + sizeof(FloatBlock) - 1) & ~(sizeof(FloatBlock) - 1)))
#define _IR_NEURO_BLOCK_DOWNALIGN(_VALUE) ((FloatBlock*)(((size_t)_VALUE) & ~(sizeof(FloatBlock) - 1)))

namespace ir
{
	class TanhFunction
	{
	public:
		static inline float function(const float input);
		static inline float derivative(const float output);
	};

	class ReLUFunction
	{
	public:
		static inline float function(const float input);
		static inline float derivative(const float output);
	};

	template <class ActivationFunction = TanhFunction, unsigned int Align = 1> class Neuro
	{
	private:
		struct FileHeader
		{
			char signature[3]		= { 'I', 'N', 'R' };
			unsigned char version	= 1;
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
		float *_goal			= nullptr;
		float _coefficient		= 0.0f;

		const float *_userinput	= nullptr;
		const float *_usergoal	= nullptr;
		float *_useroutput		= nullptr;
		
		ec _init(unsigned int nlayers, const unsigned int *layers, float amplitude, FILE *file);

		static void _stepforward(
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix,
			unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
			unsigned int nextlen, float *IR_NEURO_CRITICAL_RESTRICT nextvector);
		static void _lastbackward(
			unsigned int lastlen,
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT goal,
			const FloatBlock *IR_NEURO_CRITICAL_RESTRICT lastoutput,
			FloatBlock *IR_NEURO_CRITICAL_RESTRICT lasterror);
		static void _stepbackward(
			const float *IR_NEURO_CRITICAL_RESTRICT matrix,
			unsigned int nextlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT nexterror,
			unsigned int prevlen, const float *IR_NEURO_CRITICAL_RESTRICT prevvector, float *IR_NEURO_CRITICAL_RESTRICT preverror);
		static void _corrigate(
			float coefficient,
			unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
			unsigned int nextlen, const float *IR_NEURO_CRITICAL_RESTRICT nexterror,
			FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix);
		
		static void _freevectors(float **vector, unsigned int n);

	public:
		Neuro(unsigned int nlayers, const unsigned int *layers, float amplitude, ec *code);
		Neuro(const syschar *filepath, ec *code);
		ec set_input(const float *input, bool copy = false);
		ec set_goal(const float *goal, bool copy = false);
		ec set_coefficient(float coefficient);
		ec set_output_pointer(float *output);
		ec get_output(bool copy = false) const;
		ec forward();
		ec backward();
		ec save(const syschar *filepath) const;
		~Neuro();
	};
};

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_IMPLEMENT)) && !defined(IR_NEURO_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_implementation.h>
#endif

#if ((defined(IR_IMPLEMENT) || defined(IR_NEURO_CRITICAL_IMPLEMENT)) && !defined(IR_NEURO_CRITICAL_NOT_IMPLEMENT))
	#include <implementation/ir_neuro_critical_implementation.h>
#endif

#endif	//#ifndef IR_NEURO