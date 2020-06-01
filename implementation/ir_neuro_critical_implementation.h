/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_NEURO_CRITICAL_IMPLEMENTATION
#define IR_NEURO_CRITICAL_IMPLEMENTATION

float ir::TanhFunction::function(const float input)
{
	return tanhf(input);
};

float ir::TanhFunction::derivative(const float output)
{
	return 1 - output * output;
};

float ir::ReLUFunction::function(const float input)
{
	return input >= 0 ? input : 0.01f * input;
};

float ir::ReLUFunction::derivative(const float output)
{
	return output >= 0 ? 1.0f : 0.01f;
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_stepforward(
	const FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix,
	unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
	unsigned int nextlen, float *IR_NEURO_CRITICAL_RESTRICT nextvector)
{
	unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
	for (unsigned int line = 0; line < nextlen; line++)
	{
		FloatBlock sum; for (unsigned a = 0; a < Align; a++) sum.f[a] = 0.0f;
		
		for (unsigned int columnblock = 0; columnblock < prevlenblock; columnblock++)
		{
			for (unsigned a = 0; a < Align; a++)
				sum.f[a] += matrix[line * prevlenblock + columnblock].f[a] * prevvector[columnblock].f[a];
		}

		float endsum = 0.0f;
		for (unsigned a = 0; a < Align; a++) endsum += sum.f[a];

		nextvector[line] = ActivationFunction::function(endsum);
	}
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_lastbackward(
	unsigned int lastlen,
	const FloatBlock *IR_NEURO_CRITICAL_RESTRICT goal,
	const FloatBlock *IR_NEURO_CRITICAL_RESTRICT lastvector,
	FloatBlock *IR_NEURO_CRITICAL_RESTRICT lasterror)
{
	unsigned int lastlenblock = (lastlen + Align - 1) / Align;
	for (unsigned int lineblock = 0; lineblock < lastlenblock; lineblock++)
	{
		for (unsigned int a = 0; a < Align; a++)
			lasterror[lineblock].f[a] = ActivationFunction::derivative(lastvector[lineblock].f[a])
			* (goal[lineblock].f[a] - lastvector[lineblock].f[a]);
	}
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_stepbackward(
	const float *IR_NEURO_CRITICAL_RESTRICT matrix,
	unsigned int nextlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT nexterror,
	unsigned int prevlen, const float *IR_NEURO_CRITICAL_RESTRICT prevvector, float *IR_NEURO_CRITICAL_RESTRICT preverror)
{
	unsigned int prevfloatlen = _IR_NEURO_UPALIGN(Align, prevlen);
	for (unsigned int column = 0; column < prevlen; column++)
	{
		FloatBlock sum; for (unsigned a = 0; a < Align; a++) sum.f[a] = 0.0f;
		
		unsigned int line = 0;
		while ((line + Align - 1) < nextlen)
		{
			FloatBlock v;
			for (unsigned int a = 0; a < Align; a++) v.f[a] = matrix[prevfloatlen * (line + a) + column];
			for (unsigned int a = 0; a < Align; a++) sum.f[a] += v.f[a] * nexterror[line].f[a];
			line += Align;
		}
		
		if (line < nextlen)
		{
			FloatBlock v;
			unsigned int i = 0;
			while (line + i < nextlen) { v.f[i] = matrix[prevfloatlen * (line + i) + column]; i++; }
			while (i < Align) { v.f[i] = 0.0f; i++; }
			for (unsigned int a = 0; a < Align; a++) sum.f[a] += v.f[a] * nexterror[line].f[a];
		}

		float enderror = 0.0f;
		for (unsigned a = 0; a < Align; a++) enderror += sum.f[a];
		preverror[column] = ActivationFunction::derivative(prevvector[column]) * enderror;
	}
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_corrigate(
	float coefficient,
	unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
	unsigned int nextlen, const float *IR_NEURO_CRITICAL_RESTRICT nexterror,
	FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix)
{
	unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
	for (unsigned int line = 0; line < nextlen; line++)
	{
		for (unsigned int columnblock = 0; columnblock < prevlenblock; columnblock++)
		{
			for (unsigned int a = 0; a < Align; a++)
				matrix[line * prevlenblock + columnblock].f[a] +=
				coefficient * prevvector[columnblock].f[a] * nexterror[line];
		}
	}
};

#endif	//IR_NEURO_CRITICAL_IMPLEMENTATION