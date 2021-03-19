/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <stdlib.h>
#include <assert.h>
#include <new>

template<class T>
typename ir::QuietList<T>::Element *ir::QuietList<T>::_find(size_t i) const noexcept
{
	if (i == _lastn) return _lastp;
	
	Element *iter;
	if (i > _lastn)
	{
		if (i > (_lastn + _head->size) / 2)
		{
			//iterate down from tail
			iter = _head->previous;
			for (size_t j = 0; j < _head->size - 1 - i; j++) iter = iter->previous;
		}
		else
		{
			//iterate up from last
			iter = _lastp;
			for (size_t j = 0; j < i - _lastn; j++) iter = iter->next;
		}
	}
	else
	{
		if (i > _lastn / 2)
		{
			//iterate down from last
			iter = _lastp;
			for (size_t j = 0; j < _lastn - i; j++) iter = iter->previous;
		}
		else
		{
			//iterate up from head
			iter = _head;
			for (size_t j = 0; j < i; j++) iter = iter->next;
		}
	}
	_lastn = i;
	_lastp = iter;
	return iter;
}

template<class T>
bool ir::QuietList<T>::_detach() noexcept
{
	if (_head != nullptr && _head->refcount > 1)
	{
		size_t oldsize = size();
		const Element *olditer = _head;
		clear();
		for (size_t i = 0; i < oldsize; i++, olditer = olditer->next)
		{
			if (!push_back(olditer->element)) return false;
		}
		_head->refcount = 1;
		_head->size = oldsize;
	}
	return true;
}

template<class T>
ir::QuietList<T>::QuietList() noexcept
{
}

template<class T>
ir::QuietList<T>::QuietList(size_t newsize) noexcept
{
	resize(newsize);
}

template<class T>
ir::QuietList<T>::QuietList(const QuietList<T> &list) noexcept
{
	_head = list._head;
	#ifdef _DEBUG
		_debuglist = _head;
	#endif
	if (_head != nullptr) _head->refcount++;
	_lastn = list._lastn;
	_lastp = list._lastp;
}

template<class T>
const ir::QuietList<T> &ir::QuietList<T>::assign(const QuietList<T> &list) noexcept
{
	if (_head != list._head)
	{
		clear();
		_head = list._head;
		#ifdef _DEBUG
			_debuglist = _head;
		#endif
		if (_head != nullptr) _head->refcount++;
		_lastn = list._lastn;
		_lastp = list._lastp;
	}
	return *this;
}

template<class T>
const ir::QuietList<T> &ir::QuietList<T>::operator=(const QuietList<T> &list) noexcept
{
	return assign(list);
}

template<class T>
bool ir::QuietList<T>::resize(size_t newsize) noexcept
{
	if (_head != nullptr && _head->refcount > 1)
	{
		if (!_detach()) return false;
	}

	if (newsize > size())
	{
		for (size_t i = size(); i < newsize; i++)
		{
			if (!push_back(T())) return false;
		}
	}
	else if (newsize < size())
	{
		for (size_t i = newsize; i < size(); i++) pop_back();
	}
	return true;
}

template<class T>
void ir::QuietList<T>::clear() noexcept
{
	if (_head != nullptr)
	{
		_head->refcount--;
		if (_head->refcount == 0)
		{
			for (size_t i = 0; i < size(); i++) at(i).~T();
			free(_head);
		}
		_head = nullptr;
		#ifdef _DEBUG
			_debuglist= nullptr;
		#endif
	}
}

template<class T>
ir::QuietList<T>::~QuietList() noexcept
{
	clear();
}