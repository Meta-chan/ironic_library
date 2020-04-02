/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir::Neuro - simple single-core neuronal net

#ifndef IR_NEURO
#define IR_NEURO

#include "ir_errorcode.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

namespace ir
{
	class Neuro
	{
	private:
		#ifdef _WIN32
			typedef wchar_t syschar;
		#else
			typedef char syschar;
		#endif

		struct FileHeader
		{
			char signature[3];
			unsigned char version;
		};

		bool _ok = false;
		bool _holdinput = false;
		bool _holdoutput = false;
		bool _readybackward = false;
		unsigned int _nlayers = 0;
		unsigned int *_layers = nullptr;
		float **_weights = nullptr;
		float **_outputs = nullptr;
		float **_errors = nullptr;

		ec _init(unsigned int nlayers, const unsigned int *layers, FILE *file);

		static void _stepforward(const float *matrix, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, float *nextoutput);
		static void _lastbackward(unsigned int lastlen, const float *goal, const float *lastoutput, float *lasterror);
		static void _stepbackward(const float *matrix, unsigned int nextlen, const float *nexterror, unsigned int prevlen, const float *prevoutput, float *preverror);
		static void _corrigate(float koef, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, const float *nexterror, float *matrix);
		static void _freevectors(float **vector, unsigned int n);

	public:
		Neuro(unsigned int nlayers, const unsigned int *layers, ec *code);
		Neuro(const syschar *filepath, ec *code);
		ec forward(const float *input, float *output, bool holdinput, bool holdoutput);
		ec backward(const float *input, const float *output, const float *goal, float koef);
		ec save(const syschar *filepath);
		~Neuro();
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_NEURO_IMPLEMENT))
	#include "implementation/ir_neuro_implementation.h"
#endif

#endif	//#ifndef IR_NEURO