/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include "../include/ir/file.h"
#include <stdlib.h>
#include <random>
#include <time.h>

template <class T, size_t A, class F>
ir::ec ir::Neuro<T, A, F>::_init(T amplitude, FILE *file) noexcept
{
	//Init vectors
	try { _vectors = std::vector<Matrix<T, A>>(_layers.size()); } catch (...) { return ec::alloc; }
	for (size_t i = 0; i < _layers.size(); i++)
	{
		if (!_vectors[i].init(1, _layers[i])) return ec::alloc;
	}
	
	//Init errors
	try { _errors = std::vector<Matrix<T, A>>(_layers.size() - 1); } catch (...) { return ec::alloc; }
	for (size_t i = 0; i < _layers.size() - 1; i++)
	{
		if (!_errors[i].init(1, _layers[i + 1])) return ec::alloc;
	}
	
	//Init weights
	std::default_random_engine generator;
	generator.seed((uint32)time(nullptr));
	std::uniform_real_distribution<T> distribution(-amplitude, amplitude);
	try { _weights = std::vector<Matrix<T, A>>(_layers.size() - 1); } catch (...) { return ec::alloc; }
	if (_weights.size() < _layers.size() - 1) return ec::alloc;

	for (size_t i = 0; i < _layers.size() - 1; i++)
	{
		if (!_weights[i].init(_layers[i + 1], _layers[i] + 1)) return ec::alloc;
		for (size_t j = 0; j < _layers[i + 1]; j++)
		{
			T sum = 0.0;
			for (size_t k = 0; k < _layers[i] + 1; k++)
			{
				if (file != nullptr)
				{
					double d;
					if (fread(&d, sizeof(double), 1, file) == 0) return ec::read_file;
					_weights[i].at(j, k) = (T)d;
				}
				else
				{
					_weights[i].at(j, k) = distribution(generator);
					sum += _weights[i].at(j, k);
				}
			}
			sum /= (_layers[i] + 1);
			if (file == nullptr) for (size_t k = 0; k < _layers[i] + 1; k++)
			{
				_weights[i].at(j, k) -= sum;
			}
		}
	}

	//Init goal
	if (!_goal.init(1, _layers.back())) return ec::alloc;

	_ok = true;
	return ec::ok;
}

template <class T, size_t A, class F>
ir::Neuro<T, A, F>::Neuro(size_t nlayers, const uint32 *layers, T amplitude, ec *code) noexcept
{
	ec c = ec::ok;
	try { _layers.resize(nlayers); } catch (...) { return; }
	for (size_t i = 0; i < nlayers; i++) _layers[i] = layers[i];
	c = _init(amplitude, nullptr);
	if (code != nullptr) *code = c;
}

template <class T, size_t A, class F>
ir::Neuro<T, A, F>::Neuro(const schar *filepath, ec *code) noexcept
{
	File file(filepath, SS("rb"));
	if (!file.ok())																	{ if (code != nullptr) *code = ec::open_file; return; }
	FileHeader header, sample;
	if (file.read(&header, sizeof(FileHeader)) == 0)								{ if (code != nullptr) *code = ec::read_file; return; }
	if (memcmp(&header, &sample, sizeof(FileHeader)) != 0)							{ if (code != nullptr) *code = ec::invalid_signature; return; }
	uint32 nlayers;
	if (file.read(&nlayers, sizeof(uint32)) == 0)									{ if (code != nullptr) *code = ec::read_file; return; }
	_layers.resize(nlayers);
	if (file.read(&_layers[0], sizeof(uint32) * _layers.size()) < _layers.size())	{ if (code != nullptr) *code = ec::read_file; return; }
	ec c = _init(0.0, file.file());
	if (code != nullptr) *code = c;
}

template <class T, size_t A, class F>
bool ir::Neuro<T, A, F>::ok() const noexcept
{
	return _ok;
}

template <class T, size_t A, class F>
ir::Matrix<T, A> *ir::Neuro<T, A, F>::get_input() noexcept
{
	assert(_ok);
	return &_vectors[0];
}

template <class T, size_t A, class F>
ir::Matrix<T, A> *ir::Neuro<T, A, F>::get_output() noexcept
{
	assert(_ok);
	return &_vectors[_layers.size() - 1];
}

template <class T, size_t A, class F>
ir::Matrix<T, A> *ir::Neuro<T, A, F>::get_goal() noexcept
{
	assert(_ok);
	return &_goal;
}

template <class T, size_t A, class F>
void ir::Neuro<T, A, F>::set_coefficient(T coefficient) noexcept
{
	assert(_ok);
	assert(coefficient > 0.0);
	_coefficient = coefficient;
}

template <class T, size_t A, class F>
T ir::Neuro<T, A, F>::get_coefficient() const noexcept
{
	assert(_ok);
	return _coefficient;
}

template <class T, size_t A, class F>
void ir::Neuro<T, A, F>::forward() noexcept
{
	assert(_ok);
	for (uint32 i = 0; i < (_layers.size() - 1); i++)
		_forward(&_weights[i], &_vectors[i], &_vectors[i + 1]);
}

template <class T, size_t A, class F>
void ir::Neuro<T, A, F>::backward() noexcept
{
	assert(_ok);
	_lastbackward(&_goal, &_vectors[_layers.size() - 1], &_errors[_layers.size() - 2]);

	for (uint32 i = (uint32)_layers.size() - 2; i > 0; i--)
		_backward(&_weights[i], &_errors[i], &_vectors[i], &_errors[i - 1]);

	for (uint32 i = 0; i < (_layers.size() - 1); i++)
		_corrigate(_coefficient, &_vectors[i], &_errors[i], &_weights[i]);
}

template <class T, size_t A, class F>
ir::ec ir::Neuro<T, A, F>::save(const schar *filepath) const noexcept
{
	assert(_ok);
	File file(filepath, SS("wb"));
	if (!file.ok()) return ec::create_file;

	FileHeader header;
	if (file.write(&header, sizeof(FileHeader)) < sizeof(FileHeader)) return ec::write_file;

	size_t nlayers = _layers.size();
	if (file.write(&nlayers, sizeof(uint32)) < sizeof(uint32)) return ec::write_file;
	if (file.write(_layers.data(), sizeof(uint32) * _layers.size()) < sizeof(uint32) * _layers.size()) return ec::write_file;
	for (size_t i = 0; i < _layers.size() - 1; i++)
	{
		for (size_t j = 0; j < _layers[i + 1]; j++)
		{
			for (size_t k = 0; k < _layers[i] + 1; k++)
			{
				double d = (double)_weights[i].at(j, k);
				if (file.write(&d, sizeof(double)) < sizeof(double)) return ec::read_file;
			}
		}
	}
	return ec::ok;
}

template <class T, size_t A, class F>
ir::Neuro<T, A, F>::~Neuro() noexcept
{}

//=============================================================================================

template<class T, size_t A, class F>
void ir::Neuro<T, A, F>::_forward(const Matrix<T, A> *w, const Matrix<T, A> *pv, Matrix<T, A> *nv) noexcept
{
	assert(w->width() == pv->width() + 1);
	assert(w->height() == nv->width());
	for (size_t row = 0; row < nv->width(); row++)
	{
		Chunk<T, A> sum;
		for (size_t a = 0; a < A; a++) sum.r[a] = (T)0;

		for (size_t column = 0; column < pv->width(); column += A)
		{
			sum += w->chunk_at(row, column) * pv->chunk_at(0, column);
		}
		nv->at(0, row) = F::function(sum.sum() + w->at(row, pv->height()));
	}
}

template<class T, size_t A, class F>
void ir::Neuro<T, A, F>::_lastbackward(const Matrix<T, A> *g, const Matrix<T, A> *l, Matrix<T, A> *e) noexcept
{
	assert(g->width() == l->width());
	assert(l->width() == e->width());
	for (size_t column = 0; column < g->width(); column += A)
	{
		for (size_t p = 0; p < A; p++)
		{
			e->chunk_at(0, column).r[p] = F::derivative(l->chunk_at(0, column).r[p])
				* (g->chunk_at(0, column).r[p] - l->chunk_at(0, column).r[p]);
		}
	}
}

template<class T, size_t A, class F>
void ir::Neuro<T, A, F>::_backward(const Matrix<T, A> *w, const Matrix<T, A> *ne, const Matrix<T, A> *pv, Matrix<T, A> *pe) noexcept
{
	assert(w->width() == pv->width() + 1);
	assert(w->height() == ne->width());
	assert(pv->width() == pe->width());
	for (size_t column = 0; column < pe->width(); column++)
	{
		Chunk<T, A> sum;
		for (size_t a = 0; a < A; a++) sum.r[a] = (T)0;

		size_t row = 0;
		for (; row + A <= w->height(); row += A)
		{
			sum += w->vertical_chunk_at(row, column) * ne->chunk_at(0, row);
		}
		if (row < w->height())
		{
			sum += w->vertical_chunk_at_edge(row, column) * ne->chunk_at(0, row);
		}

		pe->at(0, column) = F::derivative(pv->at(0, column)) * sum.sum();
	}
}

template<class T, size_t A, class F>
void ir::Neuro<T, A, F>::_corrigate(T coef, const Matrix<T, A> *pv, const Matrix<T, A> *ne, Matrix<T, A> *w) noexcept
{
	assert(w->width() == pv->width() + 1);
	assert(w->height() == ne->width());
	for (size_t row = 0; row < ne->width(); row++)
	{
		Chunk<T, A> coef_chunk;
		for (size_t a = 0; a < A; a++) coef_chunk.r[a] = coef * ne->at(0, row);

		for (size_t column = 0; column < pv->height(); column += A)
		{
			w->chunk_at(row, column) += coef_chunk * pv->chunk_at(0, column);
		}
		w->at(row, pv->height()) += coef * ne->at(0, row);
	}
}