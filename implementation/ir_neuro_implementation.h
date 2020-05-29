/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_NEURO_IMPLEMENTATION
#define IR_NEURO_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <random>
#include <time.h>
#include <math.h>

#ifdef _WIN32
	#include <share.h>
#endif

#include <ir_resource/ir_memresource.h>
#include <ir_resource/ir_file_resource.h>

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

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::_init(unsigned int nlayers, const unsigned int *layers, float amplitude, FILE *file)
{
	_nlayers = nlayers;

	//Initializing _layers (n)
	if (nlayers < 2) return ec::ec_neuro_invalid_layers;
	for (unsigned int i = 0; i < nlayers; i++)
	{
		if (layers[i] == 0) return ec::ec_neuro_invalid_layers;
	}
	_layers = (unsigned int*)malloc(nlayers * sizeof(unsigned int));
	if (_layers == nullptr) return ec::ec_alloc;
	memcpy(_layers, layers, nlayers * sizeof(unsigned int));

	//Initializing _weights (n - 1)
	_weights = (float**)malloc((nlayers - 1) * sizeof(float*));
	if (_weights == nullptr) return ec::ec_alloc;
	memset(_weights, 0, (nlayers - 1) * sizeof(float));

	std::default_random_engine generator;
	generator.seed((unsigned int)time(NULL));
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	
	for (unsigned int i = 0; i < (nlayers - 1); i++)
	{
		//Marixes are always one element wider than given because of adjustment
		unsigned int weightssize = (layers[i] + 1) * layers[i + 1];
		_weights[i] = (float *)malloc(weightssize * sizeof(float));
		if (_weights[i] == nullptr) return ec::ec_alloc;
		
		if (file != nullptr)
		{ 
			if (fread(_weights[i], sizeof(float), weightssize, file) != weightssize) return ec::ec_read_file;
		}
		else
		{
			for (unsigned int k = 0; k < weightssize; k++)
			{
				_weights[i][k] = distribution(generator) * amplitude;
			}
		}
	}

	//Initializing _vectors (n)
	_vectors = (float**)malloc(nlayers * sizeof(float*));
	if (_vectors == nullptr) return ec::ec_alloc;
	memset(_vectors, 0, nlayers * sizeof(float));

	for (unsigned int i = 0; i < nlayers; i++)
	{
		_vectors[i] = (float *)malloc(layers[i] * sizeof(float));
		if (_vectors[i] == nullptr) return ec::ec_alloc;
	}

	//Initializing _errors (n - 1)
	_errors = (float**)malloc((nlayers - 1) * sizeof(float*));
	if (_errors == nullptr) return ec::ec_alloc;
	memset(_errors, 0, (nlayers - 1) * sizeof(float));

	for (unsigned int i = 0; i < (nlayers - 1); i++)
	{
		_errors[i] = (float *)malloc(layers[i + 1] * sizeof(float));
		if (_errors[i] == nullptr) return ec::ec_alloc;
	}

	_ok = true;
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::Neuro<ActivationFunction>::Neuro(unsigned int nlayers, const unsigned int *layers, float amplitude, ec *code)
{
	ec c = _init(nlayers, layers, amplitude, nullptr);
	if (code != nullptr) *code = c;
};

template <class ActivationFunction>
ir::Neuro<ActivationFunction>::Neuro(const syschar *filepath, ec *code)
{
	#ifdef _WIN32
		FileResource file = _wfsopen(filepath, L"rb", _SH_DENYNO);
	#else
		FileResource file = fopen(filepath, "rb");
	#endif

	if (file == nullptr)
	{
		if (code != nullptr) *code = ec::ec_open_file;
		return;
	}

	FileHeader header, sample;
	if (fread(&header, sizeof(FileHeader), 1, file) == 0	||
		memcmp(header.signature, sample.signature, 3) != 0	||
		header.version != sample.version)
	{
		if (code != nullptr) *code = ec::ec_invalid_signature;
		return;
	}

	unsigned int nlayers;
	if (fread(&nlayers, sizeof(unsigned int), 1, file) == 0)
	{
		if (code != nullptr) *code = ec::ec_read_file;
		return;
	}

	MemResource<unsigned int> layers = (unsigned int*)malloc(nlayers * sizeof(unsigned int));
	if (fread(layers, sizeof(unsigned int), nlayers, file) < nlayers)
	{
		if (code != nullptr) *code = ec::ec_read_file;
		return;
	}

	ec c = _init(nlayers, layers, 0.0f, file);
	if (code != nullptr) *code = c;
};

template <class ActivationFunction>
void ir::Neuro<ActivationFunction>::_stepforward(const float *matrix, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, float *nextoutput)
{
	for (unsigned int i = 0; i < nextlen; i++)
	{
		const float *row = matrix + (prevlen + 1) * i;	//+1 because first element in row is adjustment
		float sum = *row;								//getting adjustment
		row++;
		for (unsigned int j = 0; j < prevlen; j++)
		{
			sum += row[j] * prevoutput[j];
		}
		nextoutput[i] = ActivationFunction::function(sum);
	}
};

template <class ActivationFunction>
void ir::Neuro<ActivationFunction>::_lastbackward(unsigned int lastlen, const float *goal, const float *lastoutput, float *lasterror)
{
	for (unsigned int i = 0; i < lastlen; i++)
	{
		lasterror[i] = ActivationFunction::derivative(lastoutput[i]) * (goal[i] - lastoutput[i]);
	}
};

template <class ActivationFunction>
void ir::Neuro<ActivationFunction>::_stepbackward(const float *matrix, unsigned int nextlen, const float *nexterror, unsigned int prevlen, const float *prevoutput, float *preverror)
{
	for (unsigned int i = 0; i < prevlen; i++)
	{
		float error = 0.0f;
		const float *column = matrix + i + 1;					//+1 because the first element is adjustment, we ignore it
		for (unsigned int j = 0; j < nextlen; j++)
		{
			error += column[(prevlen + 1) * j] * nexterror[j];	//same reason
		}
		preverror[i] = ActivationFunction::derivative(prevoutput[i]) * error;
	}
};

template <class ActivationFunction>
void ir::Neuro<ActivationFunction>::_corrigate(float coefficient, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, const float *nexterror, float *matrix)
{
	for (unsigned int i = 0; i < nextlen; i++)
	{
		float error = nexterror[i];
		float *row = matrix + (prevlen + 1) * i;	//first element is adjustment, output is always 1 for it
		(*row) += coefficient * error;
		row++;
		for (unsigned int j = 0; j < prevlen; j++)
		{
			row[j] += coefficient * prevoutput[j] * error;
		}
	}
};

template <class ActivationFunction>
void ir::Neuro<ActivationFunction>::_freevectors(float **vector, unsigned int n)
{
	for (unsigned int i = 0; i < n; i++)
	{
		if (vector[i] == nullptr) return;
		else free(vector[i]);
	}
	free(vector);
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::set_input(const float *input, bool copy)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (copy)
	{
		memcpy(_vectors[0], input, _layers[0] * sizeof(float));
		_userinput = nullptr;
	}
	else _userinput = input;
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::set_goal(const float *goal, bool copy)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (copy)
	{
		memcpy(_vectors[_nlayers - 1], goal, _layers[_nlayers - 1] * sizeof(float));
		_usergoal = nullptr;
	}
	else _usergoal = goal;
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::set_coefficient(float coefficient)
{
	if (!_ok) return ec::ec_object_not_ok;
	_coefficient = coefficient;
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::set_output_pointer(float *output)
{
	if (!_ok) return ec::ec_object_not_ok;
	_useroutput = output;
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::get_output(bool copy) const
{
	if (!_ok) return ec::ec_object_not_ok;
	if (copy) memcpy(_useroutput, _vectors[_nlayers - 1], _layers[_nlayers - 1] * sizeof(float));
	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::forward()
{
	if (!_ok) return ec::ec_object_not_ok;
	
	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		//Calculating output for latout [i + 1] from [i]
		_stepforward(_weights[i],
			_layers[i],
			(i == 0 && _userinput != nullptr) ? _userinput : _vectors[i],
			_layers[i + 1],
			(i == (_nlayers - 2) && _useroutput != nullptr) ? _useroutput : _vectors[i + 1]);
	}

	return ec::ec_ok;
}

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::backward()
{
	if (!_ok) return ec::ec_object_not_ok;
	
	//Calculating error for layer [_nlayer - 1] from target and output of layer [_nlayer - 1]
	_lastbackward(_layers[_nlayers - 1],
		_usergoal != nullptr ? _usergoal : _goal,
		_useroutput != nullptr ? _useroutput : _vectors[_nlayers - 1],
		_errors[_nlayers - 2]);

	for (unsigned int i = _nlayers - 2; i > 0; i--)
	{
		//Calculating error for layer [i] from output of [i] and error of [i + 1]
		_stepbackward(_weights[i],
			_layers[i + 1],
			_errors[i],
			_layers[i],
			_vectors[i],
			_errors[i - 1]);
	}

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		//Corrigating weights for matrix between [i] and [i + 1] from output of layer [i] and error of [i + 1]
		_corrigate(_coefficient,
			_layers[i],
			(i == 0 && _userinput != nullptr) ? _userinput : _vectors[i],
			_layers[i + 1],
			_errors[i],
			_weights[i]);
	}

	return ec::ec_ok;
};

template <class ActivationFunction>
ir::ec ir::Neuro<ActivationFunction>::save(const syschar *filepath) const
{
	if (!_ok) return ec::ec_object_not_ok;

	#ifdef _WIN32
		FileResource file = _wfsopen(filepath, L"wb", _SH_DENYNO);
	#else
		FileResource file = fopen(filepath, "wb");
	#endif

	if (file == nullptr) return ec::ec_create_file;

	FileHeader header;
	if (fwrite(&header, sizeof(FileHeader), 1, file) == 0) return ec::ec_write_file;

	if (fwrite(&_nlayers, sizeof(unsigned int), _nlayers, file) < _nlayers) return ec::ec_write_file;

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		unsigned int matrixsize = (_layers[i] + 1) * _layers[i + 1];
		if (fwrite(_weights[i], sizeof(float), matrixsize, file) < matrixsize) return ec::ec_write_file;
	}

	return ec::ec_ok;
};

template <class ActivationFunction>
ir::Neuro<ActivationFunction>::~Neuro()
{
	if (_vectors != nullptr) _freevectors(_vectors, _nlayers);
	if (_errors != nullptr) _freevectors(_errors, _nlayers - 1);
	if (_weights != nullptr) _freevectors(_weights, _nlayers - 1);
	if (_goal != nullptr) free(_goal);
	if (_layers != nullptr) free(_layers);
};

#endif	//#ifndef IR_NEURO_IMPLEMENTATION