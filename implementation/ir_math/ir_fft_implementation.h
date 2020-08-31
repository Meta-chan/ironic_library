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

template <class T> bool ir::fft(std::complex<T> *data, const size_t size)
{
	if (size == 0 || (size & (size - 1)) != 0) return false;

	unsigned int k = size;
	T thetaT = (T)M_PI / size;

	std::complex<T> phiT(cos(thetaT), sin(thetaT));
	while (k > 1)
	{
		unsigned int n = k;
		k >>= 1;
		phiT *= phiT;
		std::complex<T> t0(1.0, 0.0);
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < size; a += n)
			{
				unsigned int b = a + k;
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
	
	for (unsigned int a = 0; a < size; a++)
	{
		unsigned int b = a;
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
};

template<class T> bool ir::ifft(std::complex<T> *data, size_t size)
{
	if (size == 0 || (size & (size - 1)) != 0) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	if (!fft(data, size)) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]) / (T)size;
	return true;
};

template<class T> bool ir::ifft_nodivide(std::complex<T> *data, size_t size)
{
	if (size == 0 || (size & (size - 1)) != 0) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	if (!fft(data, size)) return false;
	for (size_t i = 0; i < size; i++) data[i] = conj(data[i]);
	return true;
};

#endif	//#ifndef IR_FFT_IMPLEMENTATION