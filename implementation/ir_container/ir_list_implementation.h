/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_LIST_IMPLEMENTATION
#define IR_LIST_IMPLEMENTATION

#include <stdexcept>
#include <stdlib.h>
#include <string.h>

template<class T> char const * const ir::List<T>::_out_of_range_message = "ir::List out of range";

template<class T> typename ir::List<T>::Element *ir::List<T>::_find(size_t i) const
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
};

template<class T> ir::List<T>::List() noexcept
{
};

template<class T> ir::List<T>::List(size_t newsize)
{
	resize(newsize);
};

template<class T> ir::List<T>::List(const List &list) noexcept
{
	clear();
	_head = list._head;
	#ifdef _DEBUG
		_debuglist = _head;
	#endif
	if (_head != nullptr) _head->refcount++;
	_lastn = list._lastn;
	_lastp = list._lastp;
};

template<class T> T &ir::List<T>::operator[](size_t i)
{
	return at(i);
};

template<class T> T &ir::List<T>::at(size_t i)
{
	if (_head == nullptr || i >= _head->size) throw std::out_of_range(_out_of_range_message);
	if (_head->refcount > 1) detach();
	return _find(i)->element;
};

template<class T> T &ir::List<T>::front()
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	return _head->element;
};

template<class T> T &ir::List<T>::back()
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	return _head->previous->element;
};

template<class T> const T &ir::List<T>::operator[](size_t i) const
{
	return at(i);
};

template<class T> const T &ir::List<T>::at(size_t i) const
{
	if (_head == nullptr || i >= _head->size) throw std::out_of_range(_out_of_range_message);
	return _find(i)->element;
};

template<class T> const T &ir::List<T>::front() const
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	return _head->element;
};

template<class T> const T &ir::List<T>::back() const
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	return _head->previous->element;
};

template<class T> bool ir::List<T>::empty() const noexcept
{
	return (_head == nullptr || _head->size == 0);
};

template<class T> size_t ir::List<T>::size() const noexcept
{
	return (_head == nullptr ? 0 : _head->size);
};

template<class T> void ir::List<T>::resize(size_t newsize)
{
	if (newsize > size())
	{
		T dummy;
		for (size_t i = size(); i < newsize; i++)	push_back(dummy);
	}
	else if (newsize < size())
	{
		for (size_t i = newsize; i < size(); i++)	pop_back();
	}
};

template<class T> void ir::List<T>::push_back(const T &elem)
{
	if (_head != nullptr && _head->refcount > 1) detach();
	if (_head == nullptr)
	{
		_head = (Head*)malloc(sizeof(Head));//allocate
		#ifdef _DEBUG
			_debuglist = _head;
		#endif
		if (_head == nullptr) throw std::bad_alloc();
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
		if (newtail == nullptr) throw std::bad_alloc();
		newtail->next = nullptr;			//set pointers
		newtail->previous = oldtail;
		oldtail->next = newtail;
		_head->previous = newtail;			//insert
		_head->size++;						//manage global
		new(&newtail->element) T(elem);		//create
	}
};

template<class T> void ir::List<T>::pop_back()
{
	if (size() == 0) throw std::out_of_range(_out_of_range_message);
	if (_head->refcount > 1) detach();

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
};
	
template<class T> void ir::List<T>::insert(size_t i, const T &elem)
{
	if (i > size()) throw std::out_of_range(_out_of_range_message);
	if (_head != nullptr && _head->refcount > 1) detach();
	
	if (i == size()) push_back(elem);
	{
		Element *oldielem1 = _find(i);								//allocate
		Element *oldielem2 = oldielem1->next;
		Element *newielem = (Element*)malloc(sizeof(Element));
		if (newielem == nullptr) throw std::bad_alloc();
		oldielem2->previous = newielem;								//set pointers
		oldielem1->next = newielem;
		newielem->next = oldielem2;
		newielem->previous = oldielem1;
		_head->size++;												//manage global
		new(&newielem->element) T(elem);							//create
		if (_lastn >= i) _lastn++;									//manage last
	}
};

template<class T> void ir::List<T>::erase(size_t i)
{
	if (i >= size()) throw std::out_of_range(_out_of_range_message);
	if (_head->refcount > 1) detach();
	
	if (i == size() - 1) pop_back();
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
};

template<class T> const ir::List<T> &ir::List<T>::operator=(const List<T> &list) noexcept
{
	clear();
	_head = list._head;
	#ifdef _DEBUG
		_debuglist= _head;
	#endif
	if (_head != nullptr) _head->refcount++;
	_lastn = list._lastn;
	_lastp = list._lastp;
	return *this;
};

template<class T> void ir::List<T>::clear() noexcept
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
};

template<class T> void ir::List<T>::detach()
{
	if (_head != nullptr && _head->refcount > 1)
	{
		if (size() > 0)
		{
			size_t oldsize = size();
			const Element *olditer = _head;
			clear();
			for (size_t i = 0; i < oldsize; i++, olditer = olditer->next) push_back(olditer->element);
			_head->refcount = 1;
			_head->size = oldsize;
		}
		else clear();
	}
};

template<class T> ir::List<T>::~List() noexcept
{
	clear();
};

#endif //#ifndef IR_LIST_IMPLEMENTATION