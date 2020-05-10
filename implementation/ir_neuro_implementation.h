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
#include <ir_resource.h>

class MemFreer { public: static void free(void *mem) { ::free(mem); } };
typedef ir::Resource<float*, MemFreer, nullptr> FloatResource;
typedef ir::Resource<char*, MemFreer, nullptr> CharResource;
typedef ir::Resource<unsigned int*, MemFreer, nullptr> UintResource;

class FileFreer { public: static void free(FILE *file) { ::fclose(file); } };
typedef ir::Resource<FILE*, FileFreer, nullptr> FileResource;

ir::ec ir::Neuro::_init(unsigned int nlayers, const unsigned int *layers, FILE *file)
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
	generator.seed(time(NULL));
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
				_weights[i][k] = distribution(generator);
			}
		}
	}

	//Initializing _outputs (n)
	_outputs = (float**)malloc(nlayers * sizeof(float*));
	if (_outputs == nullptr) return ec::ec_alloc;
	memset(_outputs, 0, nlayers * sizeof(float));

	for (unsigned int i = 0; i < nlayers; i++)
	{
		_outputs[i] = (float *)malloc(layers[i] * sizeof(float));
		if (_outputs[i] == nullptr) return ec::ec_alloc;
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

ir::Neuro::Neuro(unsigned int nlayers, const unsigned int *layers, ec *code)
{
	ec c = _init(nlayers, layers, nullptr);
	if (code != nullptr) *code = c;
};

ir::Neuro::Neuro(const syschar *filepath, ec *code)
{
	#ifdef _WIN32
		FileResource file = _wfopen(filepath, L"rb");
	#else
		FileResource file = fopen(filepath, "rb");
	#endif

	if (file.it == nullptr)
	{
		if (code != nullptr) *code = ec::ec_open_file;
		return;
	}

	FileHeader header;
	if (fread(&header, sizeof(FileHeader), 1, file.it) == 0 ||
		memcmp(header.signature, "INR", 3) != 0 ||
		header.version != 0)
	{
		if (code != nullptr) *code = ec::ec_invalid_signature;
		return;
	}

	unsigned int nlayers;
	if (fread(&nlayers, sizeof(unsigned int), 1, file.it) == 0)
	{
		if (code != nullptr) *code = ec::ec_read_file;
		return;
	}

	UintResource layers = (unsigned int*)malloc(nlayers * sizeof(unsigned int));
	if (fread(layers.it, sizeof(unsigned int), nlayers, file.it) < nlayers)
	{
		if (code != nullptr) *code = ec::ec_read_file;
		return;
	}

	ec c = _init(nlayers, layers.it, file.it);
	if (code != nullptr) *code = c;
};

void ir::Neuro::_freevectors(float **vector, unsigned int n)
{
	for (unsigned int i = 0; i < n; i++)
	{
		if (vector[i] == nullptr) return;
		else free(vector[i]);
	}
	free(vector);
};

void ir::Neuro::_stepforward(const float *matrix, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, float *nextoutput)
{
	for (unsigned int i = 0; i < nextlen; i++)
	{
		const float *row = matrix + (prevlen + 1) * i;	//+1 because first element in row is adjustment
		float output = *row;							//getting adjustment
		row++;
		for (unsigned int j = 0; j < prevlen; j++)
		{
			output += row[j] * prevoutput[j];
		}
		nextoutput[i] = tanhf(output);
	}
};

void ir::Neuro::_lastbackward(unsigned int lastlen, const float *goal, const float *lastoutput, float *lasterror)
{
	for (unsigned int i = 0; i < lastlen; i++)
	{
		float out = lastoutput[i];
		lasterror[i] = (1 - out * out) * (goal[i] - out);
	}
};

void ir::Neuro::_stepbackward(const float *matrix, unsigned int nextlen, const float *nexterror, unsigned int prevlen, const float *prevoutput, float *preverror)
{
	for (unsigned int i = 0; i < prevlen; i++)
	{
		float error = 0.0f;
		const float *column = matrix + i + 1;					//+1 because the first element is adjustment, we ignore it
		for (unsigned int j = 0; j < nextlen; j++)
		{
			error += column[(prevlen + 1) * j] * nexterror[j];	//same reason
		}
		float output = prevoutput[i];
		preverror[i] = (1 - output * output) * error;
	}
};

void ir::Neuro::_corrigate(float koef, unsigned int prevlen, const float *prevoutput, unsigned int nextlen, const float *nexterror, float *matrix)
{
	for (unsigned int i = 0; i < nextlen; i++)
	{
		float error = nexterror[i];
		float *row = matrix + (prevlen + 1) * i;	//first element is adjustment, output is always 1 for it
		(*row) += koef * error;
		row++;
		for (unsigned int j = 0; j < prevlen; j++)
		{
			row[j] += koef * prevoutput[j] * error;
		}
	}
};

ir::ec ir::Neuro::forward(const float *input, float *output, bool holdinput, bool holdoutput)
{
	if (!_ok) return ec::ec_object_not_ok;
	if (holdinput) memcpy(_outputs[0], input, _layers[0] * sizeof(float));
	_holdinput = holdinput;

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		//Calculating output for latout [i + 1] from [i]
		_stepforward(_weights[i], _layers[i], (i == 0) ? input : _outputs[i], _layers[i + 1], (i == _nlayers - 2) ? output : _outputs[i + 1]);
	}

	if (holdoutput) memcpy(_outputs[0], output, _layers[_nlayers - 1] * sizeof(float));
	_holdoutput = holdoutput;

	_readybackward = true;
	return ec::ec_ok;
}

ir::ec ir::Neuro::backward(const float *input, const float *output, const float *goal, float koef)
{
	if (!_ok) return ec::ec_object_not_ok;
	if ((!_readybackward) || (input == nullptr && !_holdinput) || (output == nullptr && !_holdoutput)) return ec::ec_neuro_invalid_forward;
	_readybackward = false;

	//Calculating error for layer [_nlayer - 1] from target and output of layer [_nlayer - 1]
	_lastbackward(_layers[_nlayers - 1], goal, output, _errors[_nlayers - 2]);

	for (unsigned int i = _nlayers - 2; i > 0; i--)
	{
		//Calculating error for layer [i] from output of [i] and error of [i + 1]
		_stepbackward(_weights[i], _layers[i + 1], _errors[i], _layers[i], _outputs[i], _errors[i - 1]);
	}

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		//Corrigating weights for matrix between [i] and [i + 1] from output of layer [i] and error of [i + 1]
		_corrigate(koef, _layers[i], (i == 0 && !_holdinput) ? input : _outputs[i], _layers[i + 1], _errors[i], _weights[i]);
	}

	return ec::ec_ok;
};

ir::ec ir::Neuro::save(const syschar *filepath)
{
	if (!_ok) return ec::ec_object_not_ok;

	#ifdef _WIN32
		FileResource file = _wfopen(filepath, L"wb");
	#else
		FileResource file = fopen(filepath, "wb");
	#endif

	if (file.it == nullptr) return ec::ec_create_file;

	FileHeader header;
	memcpy(header.signature, "INR", 3);
	header.version = 0;
	if (fwrite(&header, sizeof(FileHeader), 1, file.it) == 0) return ec::ec_write_file;

	if (fwrite(&_nlayers, sizeof(unsigned int), _nlayers, file.it) < _nlayers) return ec::ec_write_file;

	for (unsigned int i = 0; i < (_nlayers - 1); i++)
	{
		unsigned int matrixsize = (_layers[i] + 1) * _layers[i + 1];
		if (fwrite(_weights[i], sizeof(float), matrixsize, file.it) < matrixsize) return ec::ec_write_file;
	}

	return ec::ec_ok;
};

ir::Neuro::~Neuro()
{
	if (_outputs != nullptr) _freevectors(_outputs, _nlayers);
	if (_errors != nullptr) _freevectors(_errors, _nlayers - 1);
	if (_weights != nullptr) _freevectors(_weights, _nlayers - 1);
	if (_layers != nullptr) free(_layers);
};

#endif	//#ifndef IR_NEURO_IMPLEMENTATION