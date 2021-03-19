/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//Copied from somewhere on the Internet and stylized

#include "../include/ir/constants.h"

template <class T> bool ir::fft(std::complex<T> *data, const size_t size) noexcept
{
	if (size == 0 || (size & (size - 1)) != 0) return false;

	size_t k = size;
	T thetaT = (T)pi / size;

	std::complex<T> phiT(cos(thetaT), sin(thetaT));
	while (k > 1)
	{
		size_t n = k;
		k >>= 1;
		phiT *= phiT;
		std::complex<T> t0(1.0, 0.0);
		for (uint32 l = 0; l < k; l++)
		{
			for (size_t a = l; a < size; a += n)
			{
				size_t b = a + k;
				std::complex<T> t = data[a] - data[b];
				data[a] = data[a] + data[b];
				data[b] = t * t0;
			}
			t0 *= phiT;
		}
	}

	size_t log = 0;
	size_t tmpsize = size;
	while (tmpsize > 1)
	{
		log++;
		tmpsize >>= 1;
	}
	
	for (uint32 a = 0; a < size; a++)
	{
		uint32 b = a;
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - log);
		if (b > a)
		{
			std::complex<T> t = data[a];
			data[a] = data[b];
			data[b] = t;
		}
	}

	return true;
}

template<class T> bool ir::ifft(std::complex<T> *data, size_t size) noexcept
{
	if (size == 0 || (size & (size - 1)) != 0) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	if (!fft(data, size)) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]) / (T)size;
	return true;
}

template<class T> bool ir::ifft_nodivide(std::complex<T> *data, size_t size) noexcept
{
	if (size == 0 || (size & (size - 1)) != 0) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	if (!fft(data, size)) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	return true;
}