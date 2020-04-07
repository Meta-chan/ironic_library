/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Fast Fourier transformation
//Copied from somewhere on the Internet and stylized

#ifndef IR_FFT_IMPLEMENTATION
#define IR_FFT_IMPLEMENTATION

#define _USE_MATH_DEFINES
#include <math.h>

#define cbuild _Cbuild

#define cmulc _Cmulcc

_Dcomplex cadd(_Dcomplex _X, _Dcomplex _Y)
{
	return cbuild(creal(_X) + creal(_Y), cimag(_X) + cimag(_Y));
};

_Dcomplex csub(_Dcomplex _X, _Dcomplex _Y)
{
	return cbuild(creal(_X) - creal(_Y), cimag(_X) - cimag(_Y));
};

_Dcomplex cdiv(_Dcomplex _X, double _Y)
{
	return cbuild(creal(_X) / _Y, cimag(_X) / _Y);
};

unsigned int intlog2(unsigned int n)
{
	unsigned int mask = 1;
	for (unsigned int i = 0; i < 8 * sizeof(unsigned int); i++, mask <<= 1)
	{
		n = n & ~mask;
		if (n == 0) return i;
	}
};

void ir::fft(_Dcomplex *data, unsigned int N)
{
	unsigned int k = N;
	double thetaT = M_PI / N;

	_Dcomplex phiT = cbuild(cos(thetaT), sin(thetaT));
	while (k > 1)
	{
		unsigned int n = k;
		k >>= 1;
		phiT = cmulc(phiT, phiT);
		_Dcomplex T = cbuild(1.0, 0.0);
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				_Dcomplex t = csub(data[a], data[b]);
				data[a] = cadd(data[a], data[b]);
				data[b] = cmulc(t, T);
			}
			T = cmulc(T, phiT);
		}
	}

	unsigned int m = intlog2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			_Dcomplex t = data[a];
			data[a] = data[b];
			data[b] = t;
		}
	}
};

void ir::ifft(_Dcomplex *data, unsigned int N)
{
	for (unsigned int i = 0; i < N; i++) data[i] = conj(data[i]);
	fft(data, N);
	for (unsigned int i = 0; i < N; i++) data[i] = cdiv(conj(data[i]), N);
};

#endif	//#ifndef IR_FFT_IMPLEMENTATION