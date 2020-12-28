/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_VECTOR_IMPLEMENTATION
#define IR_VECTOR_IMPLEMENTATION

#include <stdexcept>

template<class T> ir::Vector<T>::Vector() noexcept
{
}

template<class T> ir::Vector<T>::Vector(size_t newsize)
{
	resize(newsize);
}

template<class T> ir::Vector<T>::Vector(const QuietVector<T> &vector) noexcept : QuietVector<T>(vector)
{
}

template <class T> T *ir::Vector<T>::data()
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
	return QuietVector<T>::data();
}

template<class T> T &ir::Vector<T>::operator[](size_t i)
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
	return QuietVector<T>::at(i);
}

template<class T> T &ir::Vector<T>::at(size_t i)
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
	return QuietVector<T>::at(i);
}

template<class T> T &ir::Vector<T>::front()
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
	return QuietVector<T>::front();
}

template<class T> T &ir::Vector<T>::back()
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
	return QuietVector<T>::back();
}

template <class T> const T *ir::Vector<T>::data() const noexcept
{
	return QuietVector<T>::data();
}

template<class T> const T &ir::Vector<T>::operator[](size_t i) const noexcept
{
	return QuietVector<T>::at(i);
}

template<class T> const T &ir::Vector<T>::at(size_t i) const noexcept
{
	return QuietVector<T>::at(i);
}

template<class T> const T &ir::Vector<T>::front() const noexcept
{
	return QuietVector<T>::front();
}

template<class T> const T &ir::Vector<T>::back() const noexcept
{
	return QuietVector<T>::back();
}

template<class T> void ir::Vector<T>::resize(size_t newsize)
{
	if (!QuietVector<T>::resize(newsize)) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::reserve(size_t newcapacity)
{
	if (!QuietVector<T>::reserve(newcapacity)) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::push_back(const T &elem)
{
	if (!QuietVector<T>::push_back(elem)) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::pop_back()
{
	if (!QuietVector<T>::pop_back()) throw std::bad_alloc();
}
	
template<class T> void ir::Vector<T>::insert(size_t i, const T &elem)
{
	if (!QuietVector<T>::insert(i, elem)) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::erase(size_t i)
{
	if (!QuietVector<T>::erase(i)) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::detach()
{
	if (!QuietVector<T>::detach()) throw std::bad_alloc();
}

template<class T> void ir::Vector<T>::detach(size_t newcapacity)
{
	if (!QuietVector<T>::detach(newcapacity)) throw std::bad_alloc();
}

#endif //#ifndef IR_VECTOR_IMPLEMENTATION