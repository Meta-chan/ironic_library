/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_NEURO_CRITICAL_IMPLEMENTATION
#define IR_NEURO_CRITICAL_IMPLEMENTATION

#include <assert.h>
#include <math.h>

template<class T>
inline T ir::Tanh<T>::function(const T input) noexcept
{
	return tanh(input);
}

template<class T>
inline T ir::Tanh<T>::derivative(const T output) noexcept
{
	return 1 - output * output;
}

template<class T>
inline T ir::ReLU<T>::function(const T input) noexcept
{
	return input >= 0 ? input : 0.01 * input;
}

template<class T>
inline T ir::ReLU<T>::derivative(const T output) noexcept
{
	return output >= 0 ? 1.0 : 0.01;
}

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_forward(const MatrixC<T, A> *w, const VectorC<T, A> *pv, VectorC<T, A> *nv) noexcept
{
	assert(w->width() == pv->height() + 1);
	assert(w->height() == nv->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(w, pv, nv)
	#endif
	for (int row = 0; (unsigned int)row < nv->height(); row++)
	{
		BlockC<T, A> sum;
		sum.assign_zero();
		for (unsigned int columnblock = 0; columnblock < pv->block_height(); columnblock++)
		{
			sum += w->block(row, columnblock) * pv->block(columnblock);
		}
		nv->data()[row] = F::function(sum.sum() + w->at(row, pv->height()));
	}
}

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_lastbackward(const VectorC<T, A> *g, const VectorC<T, A> *l, VectorC<T, A> *e) noexcept
{
	assert(g->height() == l->height());
	assert(l->height() == e->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(g, l, e)
	#endif
	for (int rowblock = 0; (unsigned int)rowblock < g->block_height(); rowblock++)
	{
		for (unsigned int p = 0; p < A; p++)
		{
			e->block(rowblock).r[p] = F::derivative(l->block(rowblock).r[p])
				* (g->block(rowblock).r[p] - l->block(rowblock).r[p]);
		}
	}
}

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
		BlockC<T, A> sum;
		sum.assign_zero();

		unsigned int rowblock = 0;
		while (rowblock < w->complete_column_block_height())
		{
			sum += w->complete_column_block(rowblock, column) * ne->block(rowblock);
			rowblock++;
		}
		if (rowblock * A < w->height())
		{
			sum += w->column_block(rowblock, column) * ne->block(rowblock);
		}

		pe->at(column) = F::derivative(pv->at(column)) * sum.sum();
	}
}

template<class T, unsigned int A, class F>
void ir::Neuro<T, A, F>::_corrigate(T coef, const VectorC<T, A> *pv, const VectorC<T, A> *ne, MatrixC<T, A> *w) noexcept
{
	assert(w->width() == pv->height() + 1);
	assert(w->height() == ne->height());
	#ifdef IR_NEURO_CRITICAL_OPENMP
		#pragma omp parallel for firstprivate(coef, pv, ne, w)
	#endif
	for (int row = 0; (unsigned int)row < ne->height(); row++)
	{
		BlockC<T, A> rowcoef;
		for (unsigned int a = 0; a < A; a++) rowcoef.r[a] = coef * ne->at(row);

		for (unsigned int columnblock = 0; columnblock < pv->block_height(); columnblock++)
		{
			w->block(row, columnblock) += rowcoef * pv->block(columnblock);
		}
		w->at(row, pv->height()) += coef * ne->at(row);
	}
}

#endif	//#ifndef IR_NEURO_CRITICAL_IMPLEMENTATION