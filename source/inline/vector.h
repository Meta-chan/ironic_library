/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <assert.h>
#include <stdexcept>

template <class T>
inline T *ir::Vector<T>::data()
{
	if (empty()) return nullptr;
	if (_header->refcount > 1)
	{
		if (!_detach()) throw std::bad_alloc();
	}
	return (T*)(_header + 1);
}

template<class T>
inline T &ir::Vector<T>::operator[](size_t i)
{
	return at(i);
}

template<class T>
inline T &ir::Vector<T>::at(size_t i)
{
	assert(i < size());
	if (_header->refcount > 1)
	{
		if (!_detach()) throw std::bad_alloc();
	}
	return data()[i];
}

template<class T>
inline T &ir::Vector<T>::front()
{
	return at(0);
}

template<class T>
inline T &ir::Vector<T>::back()
{
	return at(size() - 1);
}

template <class T>
inline const T *ir::Vector<T>::data() const noexcept
{
	return QuietVector<T>::data();
}

template<class T>
inline const T &ir::Vector<T>::operator[](size_t i) const noexcept
{
	return QuietVector<T>::at(i);
}

template<class T>
inline const T &ir::Vector<T>::at(size_t i) const noexcept
{
	return QuietVector<T>::at(i);
}

template<class T>
inline const T &ir::Vector<T>::front() const noexcept
{
	return QuietVector<T>::front();
}

template<class T>
inline const T &ir::Vector<T>::back() const noexcept
{
	return QuietVector<T>::back();
}

template<class T>
inline void ir::Vector<T>::resize(size_t newsize)
{
	if (!QuietVector<T>::resize(newsize)) throw std::bad_alloc();
}

template<class T>
inline void ir::Vector<T>::reserve(size_t newcapacity)
{
	if (!QuietVector<T>::reserve(newcapacity)) throw std::bad_alloc();
}

template<class T>
inline void ir::Vector<T>::push_back(const T &elem)
{
	if (!QuietVector<T>::push_back(elem)) throw std::bad_alloc();
}

template<class T>
inline void ir::Vector<T>::pop_back()
{
	if (!QuietVector<T>::pop_back()) throw std::bad_alloc();
}
	
template<class T>
inline void ir::Vector<T>::insert(size_t i, const T &elem)
{
	if (!QuietVector<T>::insert(i, elem)) throw std::bad_alloc();
}

template<class T>
inline void ir::Vector<T>::erase(size_t i)
{
	if (!QuietVector<T>::erase(i)) throw std::bad_alloc();
}