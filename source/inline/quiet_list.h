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
inline T &ir::QuietList<T>::operator[](size_t i) noexcept
{
	return at(i);
}

template<class T>
inline T &ir::QuietList<T>::at(size_t i) noexcept
{
	assert(i < size());
	if (_head->refcount > 1) assert(_detach());
	return _find(i)->element;
}

template<class T>
inline T &ir::QuietList<T>::front() noexcept
{
	assert(!empty());
	if (_head->refcount > 1) assert(_detach());
	return _head->element;
}

template<class T>
inline T &ir::QuietList<T>::back() noexcept
{
	assert(!empty());
	if (_head->refcount > 1) assert(_detach());
	return _head->previous->element;
}

template<class T>
inline const T &ir::QuietList<T>::operator[](size_t i) const noexcept
{
	return at(i);
}

template<class T>
inline const T &ir::QuietList<T>::at(size_t i) const noexcept
{
	assert(i < size());
	return _find(i)->element;
}

template<class T>
inline const T &ir::QuietList<T>::front() const noexcept
{
	assert(!empty());
	return _head->element;
}

template<class T>
inline const T &ir::QuietList<T>::back() const noexcept
{
	assert(!empty());
	return _head->previous->element;
}

template<class T>
inline bool ir::QuietList<T>::empty() const noexcept
{
	return (_head == nullptr || _head->size == 0);
}

template<class T>
inline size_t ir::QuietList<T>::size() const noexcept
{
	return (_head == nullptr ? 0 : _head->size);
}

template<class T>
inline bool ir::QuietList<T>::push_back(const T &elem) noexcept
{
	if (_head != nullptr && _head->refcount > 1)
	{
		if (!_detach()) return false;
	}

	if (_head == nullptr)
	{
		_head = (Head*)malloc(sizeof(Head));//allocate
		#ifdef _DEBUG
			_debuglist = _head;
		#endif
		if (_head == nullptr) return false;
		_head->next = nullptr;				//set pointers
		_head->previous = _head;
		_head->size = 1;					//manage global
		_head->refcount = 1;
		new(&_head->element) T(elem);		//create
		_lastn = 0;							//manage last
		_lastp = _head;
	}
	else
	{
		Element *oldtail = _head->previous;	//allocate
		Element *newtail = (Element*)malloc(sizeof(Element));
		if (newtail == nullptr) return false;
		newtail->next = nullptr;			//set pointers
		newtail->previous = oldtail;
		oldtail->next = newtail;
		_head->previous = newtail;			//insert
		_head->size++;						//manage global
		new(&newtail->element) T(elem);		//create
	}
	return true;
}

template<class T>
inline bool ir::QuietList<T>::pop_back() noexcept
{
	assert(!empty());
	if (_head->refcount > 1)
	{
		if (!_detach()) return false;
	}

	if (size() == 1)
	{
		_head->element.~T();	//destory
		free(_head);			//deallocate
		_head = nullptr;		//manage global
		#ifdef _DEBUG
			_debuglist = _head;
		#endif
		_lastp = nullptr;		//manage last
		_lastn = 0;
	}
	else
	{
		Element *oldtail = _head->previous;							//set pointers
		oldtail->previous->next = nullptr;
		_head->previous = oldtail->previous;
		oldtail->element.~T();										//destroy
		free(oldtail);												//deallocate
		_head->size--;												//manage global
		if (_lastn == _head->size) { _lastn = 0; _lastp = _head; }	//manage last
	}

	return true;
}
	
template<class T>
inline bool ir::QuietList<T>::insert(size_t i, const T &elem) noexcept
{
	assert(i <= size());
	if (_head != nullptr && _head->refcount > 1)
	{
		if (!_detach()) return false;
	}
	
	if (i == size())
	{
		return push_back(elem);
	}
	else
	{
		Element *oldielem1 = _find(i);								//allocate
		Element *oldielem2 = oldielem1->next;
		Element *newielem = (Element*)malloc(sizeof(Element));
		if (newielem == nullptr) return false;
		oldielem2->previous = newielem;								//set pointers
		oldielem1->next = newielem;
		newielem->next = oldielem2;
		newielem->previous = oldielem1;
		_head->size++;												//manage global
		new(&newielem->element) T(elem);							//create
		if (_lastn >= i) _lastn++;									//manage last
	}
	return true;
}

template<class T>
inline bool ir::QuietList<T>::erase(size_t i)  noexcept
{
	assert(i < size());
	if (_head->refcount > 1)
	{
		if (!_detach()) return false;
	}
	
	if (i == size() - 1)
	{
		return pop_back();
	}
	else
	{
		Element *delelem = _find(i);			//set pointers
		Element *prevelem = delelem->previous;
		Element *nextelem = delelem->next;
		nextelem->previous = prevelem;
		prevelem->next = nextelem;
		delelem->element.~T();					//destroy
		free(delelem);							//deallocate
		_head->size--;							//manage global
		if (_lastn == i) _lastp = nextelem;		//manage last
		else if (_lastn > i) _lastn--;
	}
	return true;
}