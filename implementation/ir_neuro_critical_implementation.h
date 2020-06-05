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

#ifdef IR_NEURO_CRITICAL_3DNOW
	#include <type_traits>
	#ifdef _WIN32
		#include <intrin.h>
	#else
		#include <x86intrin.h>
	#endif	
#endif

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
	#ifdef IR_NEURO_CRITICAL_3DNOW
	if (Align == 2)
	{
		const __m64 hunderth_low = _m_from_int(0x3C23D70A);
		const __m64 zero = _m_from_int(0);

		//3DNOW IMPLEMENTATION HERE
		unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
		for (unsigned int line = 0; line < nextlen; line++)
		{
			__m64 sum = zero;
			for (unsigned int columnblock = 0; columnblock < prevlenblock; columnblock++)
			{
				sum = _m_pfadd(sum, _m_pfmul(*((__m64*)&matrix[line * prevlenblock + columnblock]),
					*((__m64*)&prevvector[columnblock])));
			}
			
			sum = _m_pfacc(sum, zero);

			//Specially for ReLU, we can compute the result here (skip _m_femms)
			if (std::is_same<ActivationFunction, ReLUFunction>::value)
			{
				__m64 comp = _m_pfcmpge(sum, zero);
				if (!_m_to_int(comp)) sum = _m_pfmul(sum, hunderth_low);
				*((int*)&nextvector[line]) = _m_to_int(sum);
			}
			//By default we save the result and call the function later (skip _m_femms)
			else
			{
				*((int*)&nextvector[line]) = _m_to_int(sum);
			}
		}
		
		_m_femms();
		if (!std::is_same<ActivationFunction, ReLUFunction>::value)
		{
			for (unsigned int line = 0; line < nextlen; line++)
				nextvector[line] = ActivationFunction::function(nextvector[line]);
		}
	}
	else
	{
	#endif
		//NORMAL IMPLEMENTATION HERE
		unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
		#ifdef IR_NEURO_CRITICAL_OPENMP
			#pragma omp parallel for firstprivate(matrix, prevvector, nextlen, nextvector, prevlenblock)
		#endif
		for (int line = 0; (unsigned int)line < nextlen; line++)
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
	#ifdef IR_NEURO_CRITICAL_3DNOW
	}
	#endif
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_lastbackward(
	unsigned int lastlen,
	const FloatBlock *IR_NEURO_CRITICAL_RESTRICT goal,
	const FloatBlock *IR_NEURO_CRITICAL_RESTRICT lastvector,
	FloatBlock *IR_NEURO_CRITICAL_RESTRICT lasterror)
{
	#ifdef IR_NEURO_CRITICAL_3DNOW
	if (Align == 2)
	{
		//3DNOW IMPLEMENTATION HERE
		const __m64 one = _m_pfacc(_m_from_int(0x3F800000), _m_from_int(0x3F800000));
		const __m64 hunderth = _m_pfacc(_m_from_int(0x3C23D70A), _m_from_int(0x3C23D70A));
		const __m64 zero = _m_from_int(0);

		unsigned int lastlenblock = (lastlen + Align - 1) / Align;
		for (unsigned int lineblock = 0; lineblock < lastlenblock; lineblock++)
		{
			//Special for Tanh
			if (std::is_same<ActivationFunction, TanhFunction>::value)
			{
				__m64 v = *((__m64*)&lastvector[lineblock]);
				__m64 g = *((__m64*)&goal[lineblock]);	
				*((__m64*)&lasterror[lineblock]) = _m_pfmul(_m_pfsub(one, _m_pfmul(v, v)), _m_pfsub(g, v));
			}
			//Special for ReLU
			else if (std::is_same<ActivationFunction, ReLUFunction>::value)
			{
				__m64 v = *((__m64*)&lastvector[lineblock]);
				__m64 g = *((__m64*)&goal[lineblock]);
				__m64 comp = _m_pfcmpge(v, zero);
				__m64 one_or_hunderth = _m_por(_m_pand(comp, one), _m_pandn(comp, hunderth));
				*((__m64*)&lasterror[lineblock]) = _m_pfmul(one_or_hunderth, _m_pfsub(g, v));
			}
			else
			{
				//NOT IMPLEMENTED!
			}
		}
		if(std::is_same<ActivationFunction, TanhFunction>::value
		|| std::is_same<ActivationFunction, ReLUFunction>::value) _m_femms();
	}
	else
	{
	#endif
		//NORMAL IMPLEMENTATION HERE
		unsigned int lastlenblock = (lastlen + Align - 1) / Align;
		#ifdef IR_NEURO_CRITICAL_OPENMP
			#pragma omp parallel for firstprivate(goal, lastvector, lasterror, lastlenblock)
		#endif
		for (int lineblock = 0; (unsigned int)lineblock < lastlenblock; lineblock++)
		{
			for (unsigned int a = 0; a < Align; a++)
			{
				lasterror[lineblock].f[a] = ActivationFunction::derivative(lastvector[lineblock].f[a])
					* (goal[lineblock].f[a] - lastvector[lineblock].f[a]);
			}
		}
	#ifdef IR_NEURO_CRITICAL_3DNOW
	}
	#endif
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_stepbackward(
	const float *IR_NEURO_CRITICAL_RESTRICT matrix,
	unsigned int nextlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT nexterror,
	unsigned int prevlen, const float *IR_NEURO_CRITICAL_RESTRICT prevvector, float *IR_NEURO_CRITICAL_RESTRICT preverror)
{
#ifdef IR_NEURO_CRITICAL_3DNOW
	if (Align == 2)
	{
		//3DNOW IMPLEMENTATION HERE
		const __m64 one_low = _m_from_int(0x3F800000);
		const __m64 hunderth_low = _m_from_int(0x3C23D70A);
		const __m64 zero = _m_from_int(0);
		const __m64 zero_low = _m_from_int(0);

		unsigned int prevfloatlen = _IR_NEURO_UPALIGN(Align, prevlen);
		unsigned int nextlenblock = nextlen / Align;
		for (unsigned int column = 0; column < prevlen; column++)
		{
			__m64 sum = zero;

			for (unsigned int lineblock = 0; lineblock < nextlenblock; lineblock++)
			{
				__m64 v = _m_pfacc(_m_from_int(*((int*)&matrix[prevfloatlen * Align * lineblock + column])),
					_m_from_int(*((int*)&matrix[prevfloatlen * (Align * lineblock + 1) + column])));
				sum = _m_pfadd(sum, _m_pfmul(v, *((__m64*)&nexterror[lineblock])));
			}

			if (nextlenblock * Align < nextlen)
			{
				__m64 v = _m_from_int(*((int*)&matrix[prevfloatlen * Align * nextlenblock + column]));
				sum = _m_pfadd(sum, _m_pfmul(v, *((__m64*)&nexterror[nextlenblock])));
			}

			sum = _m_pfacc(sum, zero);

			//Special for Tanh
			if (std::is_same<ActivationFunction, TanhFunction>::value)
			{
				__m64 v = _m_from_int(*((int*)&prevvector[column]));
				*((int*)&preverror[column]) = _m_to_int(_m_pfmul(_m_pfsub(one_low, _m_pfmul(v, v)), sum));
			}
			//Special for ReLU
			else if (std::is_same<ActivationFunction, ReLUFunction>::value)
			{
				__m64 v = _m_from_int(*((int*)&prevvector[column]));
				__m64 comp = _m_pfcmpge(v, zero_low);
				if (_m_to_int(comp)) *((int*)&preverror[column]) = _m_to_int(sum);
				else *((int*)&preverror[column]) = _m_to_int(_m_pfmul(sum, hunderth_low));
			}
			else
			{
				//NOT IMPLEMENTED!
			}
		}

		if (std::is_same<ActivationFunction, TanhFunction>::value
			|| std::is_same<ActivationFunction, ReLUFunction>::value) _m_femms();
	}
	else
	{
	#endif
		//NORMAL IMPLEMENTATION HERE
		unsigned int prevfloatlen = _IR_NEURO_UPALIGN(Align, prevlen);
		unsigned int nextlenblock = nextlen / Align;
		#ifdef IR_NEURO_CRITICAL_OPENMP
			#pragma omp parallel for firstprivate(matrix, nextlen, nexterror, prevlen, prevvector, preverror, prevfloatlen, nextlenblock)
		#endif
		for (int column = 0; (unsigned int)column < prevlen; column++)
		{
			FloatBlock sum; for (unsigned a = 0; a < Align; a++) sum.f[a] = 0.0f;

			for (unsigned int lineblock = 0; lineblock < nextlenblock; lineblock++)
			{
				FloatBlock v;
				for (unsigned int a = 0; a < Align; a++) v.f[a] = matrix[prevfloatlen * (Align * lineblock + a) + column];
				for (unsigned int a = 0; a < Align; a++) sum.f[a] += v.f[a] * nexterror[lineblock].f[a];
			}

			if (nextlenblock * Align < nextlen)
			{
				FloatBlock v; for (unsigned a = 0; a < Align; a++) v.f[a] = 0.0f;
				for (unsigned int a = 0; Align * nextlenblock + a < nextlen; a++)
					v.f[a] = matrix[prevfloatlen * (Align * nextlenblock + a) + column];
				for (unsigned int a = 0; a < Align; a++) sum.f[a] += v.f[a] * nexterror[nextlenblock].f[a];
			}

			float enderror = 0.0f;
			for (unsigned a = 0; a < Align; a++) enderror += sum.f[a];
			preverror[column] = ActivationFunction::derivative(prevvector[column]) * enderror;
		}
	#ifdef IR_NEURO_CRITICAL_3DNOW
	}
	#endif
};

template <class ActivationFunction, unsigned int Align>
void ir::Neuro<ActivationFunction, Align>::_corrigate(
	float coefficient,
	unsigned int prevlen, const FloatBlock *IR_NEURO_CRITICAL_RESTRICT prevvector,
	unsigned int nextlen, const float *IR_NEURO_CRITICAL_RESTRICT nexterror,
	FloatBlock *IR_NEURO_CRITICAL_RESTRICT matrix)
{
	#ifdef IR_NEURO_CRITICAL_3DNOW
	if (Align == 2)
	{
		//3DNOW IMPLEMENTATION HERE
		__m64 coef = _m_pfacc(_m_from_int(*((int*)&coefficient)), _m_from_int(*((int*)&coefficient)));
		unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
		for (unsigned int line = 0; line < nextlen; line++)
		{
			__m64 linecoef = _m_pfmul(coef,
				_m_pfacc(_m_from_int(*((int*)&nexterror[line])), _m_from_int(*((int*)&nexterror[line]))));
			for (unsigned int columnblock = 0; columnblock < prevlenblock; columnblock++)
			{
				*((__m64*)&matrix[line * prevlenblock + columnblock]) =
					_m_pfadd(*((__m64*)&matrix[line * prevlenblock + columnblock]),
					_m_pfmul(linecoef, *((__m64*)&prevvector[columnblock])));
			}
		}
		_m_femms();
	}
	else
	{
	#endif
		//NORMAL IMPLEMENTATION HERE
		unsigned int prevlenblock = (prevlen + 1 + Align - 1) / Align;
		#ifdef IR_NEURO_CRITICAL_OPENMP
			#pragma omp parallel for firstprivate(coefficient, prevvector, nextlen, nexterror, matrix, prevlenblock)
		#endif
		for (int line = 0; (unsigned int)line < nextlen; line++)
		{
			for (unsigned int columnblock = 0; columnblock < prevlenblock; columnblock++)
			{
				for (unsigned int a = 0; a < Align; a++)
					matrix[line * prevlenblock + columnblock].f[a] +=
					coefficient * prevvector[columnblock].f[a] * nexterror[line];
			}
		}
	#ifdef IR_NEURO_CRITICAL_3DNOW
	}
	#endif	
};

#endif	//IR_NEURO_CRITICAL_IMPLEMENTATION
