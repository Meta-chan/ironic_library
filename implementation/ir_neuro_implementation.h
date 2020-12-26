/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_NEURO_IMPLEMENTATION
#define IR_NEURO_IMPLEMENTATION

#include <stdlib.h>
#include <random>
#include <time.h>

#ifdef _WIN32
	#include <share.h>
#endif

#include <ir_resource/ir_memresource.h>
#include <ir_resource/ir_file_resource.h>

template <class T, unsigned int A, class F>
ir::ec ir::Neuro<T, A, F>::_init(T amplitude, FILE *file) noexcept
{
	//Init vectors
	if (!_vectors.resize(_layers.size())) return ec::alloc;
	for (unsigned int i = 0; i < _layers.size(); i++)
	{
		if (!_vectors[i].init(_layers[i])) return ec::alloc;
	}

	//Init errors
	if (!_errors.resize(_layers.size() - 1)) return ec::alloc;
	for (unsigned int i = 0; i < _layers.size() - 1; i++)
	{
		if (!_errors[i].init(_layers[i + 1])) return ec::alloc;
	}

	//Init weights
	std::default_random_engine generator;
	generator.seed((unsigned int)time(nullptr));
	std::uniform_real_distribution<T> distribution(-amplitude, amplitude);
	if (!_weights.resize(_layers.size() - 1)) return ec::alloc;
	for (unsigned int i = 0; i < _layers.size() - 1; i++)
	{
		if (!_weights[i].init(_layers[i] + 1, _layers[i + 1])) return ec::alloc;
		for (unsigned int j = 0; j < _layers[i + 1]; j++)
		{
			T sum = 0.0;
			for (unsigned int k = 0; k < _layers[i] + 1; k++)
			{
				if (file != nullptr)
				{
					double d;
					if (fread(&d, sizeof(double), 1, file) == 0) return ec::read_file;
					_weights[i].at(j, k) = d;
				}
				else
				{
					_weights[i].at(j, k) = distribution(generator);
					sum += _weights[i].at(j, k);
				}
			}
			sum /= (_layers[i] + 1);
			if (file == nullptr) for (unsigned int k = 0; k < _layers[i] + 1; k++)
			{
				_weights[i].at(j, k) -= sum;
			}
		}
	}

	//Init goal
	if (!_goal.init(_layers[_layers.size() - 1])) return ec::alloc;

	_ok = true;
	return ec::ok;
}

template <class T, unsigned int A, class F>
ir::Neuro<T, A, F>::Neuro(unsigned int nlayers, const unsigned int *layers, T amplitude, ec *code) noexcept
{
	ec c = ec::ok;
	if (!_layers.resize(nlayers)) c = ec::alloc;
	else
	{
		for (unsigned int i = 0; i < nlayers; i++) _layers[i] = layers[i];
		c = _init(amplitude, nullptr);
	}
	if (code != nullptr) *code = c;
}

template <class T, unsigned int A, class F>
ir::Neuro<T, A, F>::Neuro(const syschar *filepath, ec *code) noexcept
{
	#ifdef _WIN32
		FileResource file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		FileResource file = fopen(filepath, "rb");
	#endif
	
	if (file == nullptr)																	{ if (code != nullptr) *code = ec::open_file; return; }
	FileHeader header, sample;
	if (fread(&header, sizeof(FileHeader), 1, file) == 0)									{ if (code != nullptr) *code = ec::read_file; return; }
	if (memcmp(&header, &sample, sizeof(FileHeader)) != 0)									{ if (code != nullptr) *code = ec::invalid_signature; return; }
	unsigned int nlayers;
	if (fread(&nlayers, sizeof(unsigned int), 1, file) == 0)								{ if (code != nullptr) *code = ec::read_file; return; }
	_layers.resize(nlayers);
	if (fread(&_layers[0], sizeof(unsigned int), _layers.size(), file) < _layers.size())	{ if (code != nullptr) *code = ec::read_file; return; }
	ec c = _init(0.0, file);
	if (code != nullptr) *code = c;
}

template <class T, unsigned int A, class F>
bool ir::Neuro<T, A, F>::ok() const noexcept
{
	return _ok;
}

template <class T, unsigned int A, class F>
ir::VectorC<T, A> *ir::Neuro<T, A, F>::get_input() noexcept
{
	assert(_ok);
	return &_vectors[0];
}

template <class T, unsigned int A, class F>
ir::VectorC<T, A> *ir::Neuro<T, A, F>::get_output() noexcept
{
	assert(_ok);
	return &_vectors[_layers.size() - 1];
}

template <class T, unsigned int A, class F>
ir::VectorC<T, A> *ir::Neuro<T, A, F>::get_goal() noexcept
{
	assert(_ok);
	return &_goal;
}

template <class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::set_coefficient(T coefficient) noexcept
{
	assert(_ok);
	assert(coefficient > 0.0);
	_coefficient = coefficient;
}

template <class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::forward() noexcept
{
	assert(_ok);
	for (unsigned int i = 0; i < (_layers.size() - 1); i++)
		_forward(&_weights[i], &_vectors[i], &_vectors[i + 1]);
}

template <class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::backward() noexcept
{
	assert(_ok);
	_lastbackward(&_goal, &_vectors[_layers.size() - 1], &_errors[_layers.size() - 2]);

	for (unsigned int i = (unsigned int)_layers.size() - 2; i > 0; i--)
		_backward(&_weights[i], &_errors[i], &_vectors[i], &_errors[i - 1]);

	for (unsigned int i = 0; i < (_layers.size() - 1); i++)
		_corrigate(_coefficient, &_vectors[i], &_errors[i], &_weights[i]);
}

template <class T, unsigned int A, class F>
ir::ec ir::Neuro<T, A, F>::save(const syschar *filepath) const noexcept
{
	assert(_ok);

	#ifdef _WIN32
		FileResource file = _wfsopen(filepath, L"wb", _SH_DENYNO);
	#else
		FileResource file = fopen(filepath, "wb");
	#endif

	if (file == nullptr) return ec::create_file;

	FileHeader header;
	if (fwrite(&header, sizeof(FileHeader), 1, file) == 0) return ec::write_file;

	unsigned int nlayers = (unsigned int)_layers.size();
	if (fwrite(&nlayers, sizeof(unsigned int), 1, file) == 0) return ec::write_file;
	if (fwrite(_layers.data(), sizeof(unsigned int), _layers.size(), file) < _layers.size()) return ec::write_file;
	for (unsigned int i = 0; i < _layers.size() - 1; i++)
	{
		for (unsigned int j = 0; j < _layers[i + 1]; j++)
		{
			for (unsigned int k = 0; k < _layers[i] + 1; k++)
			{
				double d = _weights[i].at(j, k);
				if (fwrite(&d, sizeof(double), 1, file) == 0) return ec::read_file;
			}
		}
	}

	return ec::ok;
}

template <class T, unsigned int A, class F>
ir::Neuro<T, A, F>::~Neuro() noexcept
{}

#endif	//#ifndef IR_NEURO_IMPLEMENTATION