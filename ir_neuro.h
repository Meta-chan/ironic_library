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

#ifndef IR_NEURO
#define IR_NEURO

#include <ir_errorcode.h>
#include <ir_syschar.h>
#include <stdio.h>

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

	template <class ActivationFunction = TanhFunction> class Neuro
	{
	private:
		struct FileHeader
		{
			char signature[3]		= { 'I', 'N', 'R' };
			unsigned char version	= 0;
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

		static void _stepforward(const float *matrix, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, float *nextoutput);
		static void _lastbackward(unsigned int lastlen, const float *goal, const float *lastoutput, float *lasterror);
		static void _stepbackward(const float *matrix, unsigned int nextlen, const float *nexterror, unsigned int prevlen, const float *prevoutput, float *preverror);
		static void _corrigate(float coefficient, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, const float *nexterror, float *matrix);
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

#if (defined(IR_IMPLEMENT) || defined(IR_NEURO_IMPLEMENT))
	#include <implementation/ir_neuro_implementation.h>
#endif

#endif	//#ifndef IR_NEURO