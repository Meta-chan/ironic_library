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
	_vectors = (VectorC<T, A>*)malloc(_nlayers * sizeof(VectorC<T, A>));
	if (_vectors == nullptr) return ec::alloc;
	for (unsigned int i = 0; i < _nlayers; i++)
	{
		bool ok;
		new(_vectors + i)VectorC<T, A>(_layers[i], &ok);
		if (!ok) return ec::alloc;
	}

	//Init errors
	_errors = (VectorC<T, A>*)malloc((_nlayers - 1) * sizeof(VectorC<T, A>));
	if (_errors == nullptr) return ec::alloc;
	for (unsigned int i = 0; i < _nlayers - 1; i++)
	{
		bool ok;
		new(_errors + i)VectorC<T, A>(_layers[i + 1], &ok);
		if (!ok) return ec::alloc;
	}

	//Init weights
	std::default_random_engine generator;
	generator.seed((unsigned int)time(nullptr));
	std::uniform_real_distribution<T> distribution(-amplitude, amplitude);
	_weights = (MatrixC<T, A>*)malloc((_nlayers - 1) * sizeof(MatrixC<T, A>));
	if (_weights == nullptr) return ec::alloc;
	for (unsigned int i = 0; i < _nlayers - 1; i++)
	{
		bool ok;
		new(_weights + i)MatrixC<T, A>(_layers[i] + 1, _layers[i + 1], &ok);
		if (!ok) return ec::alloc;
		for (unsigned int j = 0; j < _layers[i] + 1; j++)
		{
			for (unsigned int k = 0; k < _layers[i + 1]; k++)
			{
				if (file != nullptr)
				{
					double d;
					if (fread(&d, sizeof(double), 1, file) == 0) return ec::read_file;
					_weights[i].data(k)[j] = d;
				}
				else
				{
					_weights[i].data(k)[j] = distribution(generator);
				}
			}
		}
	}

	//Init goal
	bool ok;
	_goal = (VectorC<T, A>*)malloc(sizeof(VectorC<T, A>));
	if (_goal == nullptr) return ec::alloc;
	new(_goal)VectorC<T, A>(_layers[_nlayers - 1], &ok);
	if (!ok) return ec::alloc;

	_ok = true;
	return ec::ok;
}

template <class T, unsigned int A, class F>
ir::Neuro<T, A, F>::Neuro(unsigned int nlayers, const unsigned int *layers, T amplitude, ec *code) noexcept
{
	_nlayers = nlayers;
	_layers = (unsigned int*)malloc(nlayers * sizeof(unsigned int));
	if (_layers == nullptr) { *code = ec::alloc; return; }
	for (unsigned int i = 0; i < nlayers; i++) _layers[i] = layers[i];
	ec c = _init(amplitude, nullptr);
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

	FileHeader header, sample;
	if (fread(&header, sizeof(FileHeader), 1, file) == 0)					{ if (code != nullptr) *code = ec::read_file; return; }
	if (memcmp(&header, &sample, sizeof(FileHeader)) != 0)					{ if (code != nullptr) *code = ec::invalid_signature; return; }
	if (fread(&_nlayers, sizeof(unsigned int), 1, file) == 0)				{ if (code != nullptr) *code = ec::read_file; return; }
	_layers = (unsigned int*)malloc(_nlayers * sizeof(unsigned int));
	if (_layers == nullptr)													{ if (code != nullptr) *code = ec::read_file; return; }
	if (fread(_layers, sizeof(unsigned int), _nlayers, file) < _nlayers)	{ if (code != nullptr) *code = ec::read_file; return; }
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
	return &_vectors[_nlayers - 1];
}

template <class T, unsigned int A, class F>
ir::VectorC<T, A> *ir::Neuro<T, A, F>::get_goal() noexcept
{
	assert(_ok);
	return _goal;
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
	for (unsigned int i = 0; i < (_nlayers - 1); i++)
		_forward(&_weights[i], &_vectors[i], &_vectors[i + 1]);
}

template <class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::backward() noexcept
{
	assert(_ok);
	_lastbackward(_goal, &_vectors[_nlayers - 1], &_errors[_nlayers - 2]);

	for (unsigned int i = _nlayers - 2; i > 0; i--)
		_backward(&_weights[i], &_errors[i], &_vectors[i], &_errors[i - 1]);

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
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

	if (fwrite(&_nlayers, sizeof(unsigned int), 1, file) == 0) return ec::write_file;
	if (fwrite(_layers, sizeof(unsigned int), _nlayers, file) < _nlayers) return ec::write_file;
	for (unsigned int i = 0; i < _nlayers - 1; i++)
	{
		for (unsigned int j = 0; j < _layers[i] + 1; j++)
		{
			for (unsigned int k = 0; k < _layers[i + 1]; k++)
			{
				double d = _weights[i].data(k)[j];
				if (fwrite(&d, sizeof(double), 1, file) == 0) return ec::write_file;
			}
		}
	}

	return ec::ok;
}

template <class T, unsigned int A, class F>
ir::Neuro<T, A, F>::~Neuro() noexcept
{
	if (_vectors != nullptr)
	{
		for (unsigned int i = 0; i < _nlayers; i++)
		{
			_vectors[i].~VectorC<T, A>();
		}
		free(_vectors);
	}

	if (_errors != nullptr)
	{
		for (unsigned int i = 0; i < _nlayers - 1; i++)
		{
			_errors[i].~VectorC<T, A>();
		}
		free(_errors);
	}

	if (_weights != nullptr)
	{
		for (unsigned int i = 0; i < _nlayers - 1; i++)
		{
			_weights[i].~MatrixC<T, A>();
		}
		free(_weights);
	}

	if (_goal != nullptr)
	{
		_goal->~VectorC<T, A>();
		free(_goal);
	}

	if (_layers != nullptr) free(_layers);
}

#endif	//#ifndef IR_NEURO_IMPLEMENTATION