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

#include <assert.h>

#ifdef IR_NEURO_CRITICAL_3DNOW
	#ifdef _WIN32
		#include <intrin.h>
	#else
		#include <x86intrin.h>
	#endif	
#endif

template<class T> T ir::Tanh<T>::function(const T input)
{
	return tanhf(input);
};

template<class T> T ir::Tanh<T>::derivative(const T output)
{
	return 1 - output * output;
};

template<class T> T ir::ReLU<T>::function(const T input)
{
	return input >= 0 ? input : 0.01 * input;
};

template<class T> T ir::ReLU<T>::derivative(const T output)
{
	return output >= 0 ? 1.0 : 0.01;
};

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_forward(const MatrixC<T, A> *w, const VectorC<T, A> *pv, VectorC<T, A> *nv) noexcept
{
	assert(w->width() == pv->height() + 1);
	assert(w->height() == nv->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(w, pv, nv)
	#endif
	for (int line = 0; (unsigned int)line < nv->height(); line++)
	{
		BlockC<T, A> sum;
		for (unsigned int p = 0; p < A; p++) sum.r[p] = 0.0;
		for (unsigned int columnblock = 0; columnblock < pv->block_height(); columnblock++)
		{
			for (unsigned int p = 0; p < A; p++)
				sum.r[p] += w->block_data(line)[columnblock].r[p] * pv->block_data()[columnblock].r[p];
		}
		T linesum = 0.0;
		for (unsigned int p = 0; p < A; p++) linesum += sum.r[p];
		linesum += w->data(line)[pv->height()];
		nv->data()[line] = F::function(linesum);
	}
};

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_lastbackward(const VectorC<T, A> *g, const VectorC<T, A> *l, VectorC<T, A> *e) noexcept
{
	assert(g->height() == l->height());
	assert(l->height() == e->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(g, l, e)
	#endif
	for (int lineblock = 0; (unsigned int)lineblock < g->block_height(); lineblock++)
	{
		for (unsigned int p = 0; p < A; p++)
		{
			e->block_data()[lineblock].r[p] = F::derivative(l->block_data()[lineblock].r[p])
				* (g->block_data()[lineblock].r[p] - l->block_data()[lineblock].r[p]);
		}
	}
};

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_backward(const MatrixC<T, A> *w, const VectorC<T, A> *ne, const VectorC<T, A> *pv, VectorC<T, A> *pe) noexcept
{
	assert(w->width() == pv->height() + 1);
	assert(w->height() == ne->height());
	assert(pv->height() == pe->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(w, ne, pv, pe)
	#endif
	for (int column = 0; (unsigned int)column < pe->height(); column++)
	{
		BlockC<T, A> sum; for (unsigned p = 0; p < A; p++) sum.r[p] = 0.0;

		unsigned int lineblock = 0;
		while (lineblock < ne->height() / A)
		{
			BlockC<T, A> v;
			for (unsigned int p = 0; p < A; p++) v.r[p] = w->data(A * lineblock + p)[column];
			for (unsigned int p = 0; p < A; p++) sum.r[p] += v.r[p] * ne->block_data()[lineblock].r[p];
			lineblock++;
		}
		if (ne->height() > A * lineblock)
		{
			BlockC<T, A> v;
			for (unsigned int p = 0; p < (ne->height() - A * lineblock); p++) v.r[p] = w->data(A * lineblock + p)[column];
			for (unsigned int p = 0; p < A; p++) sum.r[p] += v.r[p] * ne->block_data()[lineblock].r[p];
		}

		T columnerror = 0.0;
		for (unsigned p = 0; p < A; p++) columnerror += sum.r[p];
		pe->data()[column] = F::derivative(pv->data()[column]) * columnerror;
	}
};

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_corrigate(T coef, const VectorC<T, A> *pv, const VectorC<T, A> *ne, MatrixC<T, A> *w) noexcept
{
	assert(w->width() == pv->height() + 1);
	assert(w->height() == ne->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(coef, pv, ne, w)
	#endif
	for (int line = 0; (unsigned int)line < ne->height(); line++)
	{
		T linecoef = coef * ne->data()[line];
		for (unsigned int columnblock = 0; columnblock < pv->block_height(); columnblock++)
		{
			for (unsigned int p = 0; p < A; p++)
				w->block_data(line)[columnblock].r[p] += linecoef * pv->block_data()[columnblock].r[p];
		}
		w->data(line)[pv->height()] += linecoef;
	}
};

#endif	//#ifndef IR_NEURO_CRITICAL_IMPLEMENTATION