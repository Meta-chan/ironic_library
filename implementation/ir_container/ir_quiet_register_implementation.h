/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_REGISTER_IMPLEMENTATION
#define IR_QUIET_REGISTER_IMPLEMENTATION

#include <string.h>

template <class T, class C> size_t ir::QuietRegister<T, C>::_strlen(const C *key) noexcept
{
	if (sizeof(C) == 1) return strlen((const char*)key);
	else if (sizeof(C) == 2) return wcslen((const wchar_t*)key);
	else
	{
		size_t length = 0;
		while (key[length] != 0) length++;
		return length;
	}
}

template <class T, class C> C *ir::QuietRegister<T, C>::_strdup(const C *key, size_t length) noexcept
{
	C *newkey = (C*)malloc((length + 1) * sizeof(C));
	if (newkey == nullptr) return nullptr;
	memcpy(newkey, key, length * sizeof(C));
	newkey[length] = 0;
	return newkey;
}

template <class T, class C> int ir::QuietRegister<T, C>::_strcmp(const C *key1, size_t keysize1, const C *key2, size_t keysize2) noexcept
{
	if (keysize1 != keysize2) return keysize1 - keysize2;
	else if (sizeof(C) == 1) return memcmp(key1, key2, keysize1);
	else if (sizeof(C) == 2) return wmemcmp((const wchar_t*)key1, (const wchar_t*)key2, keysize1);
	else
	{
		//Memcmp may return incorrect results on big engian machines
		for (size_t i = 0; i < keysize1; i++)
		{
			if (key1[i] != key2[i]) return key1[i] - key2[i];
		}
		return 0;
	}
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::_find(const C *key, size_t length, size_t *position) const noexcept
{
	if (size() == 0)	{ *position = 0; return false;	}
	else if (size() == 1)
	{
		int cmp = _strcmp(key, length, super::at(0).key, super::at(0).keysize);
		if (cmp < 0)	{ *position = 0; return false; }
		if (cmp == 0)	{ *position = 0; _last = 0; return true; }
		else			{ *position = 1; return false; }
	}
	else if (_last < size() && _strcmp(key, length, super::at(_last).key, super::at(_last).keysize) == 0)
	{
		*position = _last;
		return true;
	}
	else
	{
		size_t left = 0;
		size_t right = size() - 1;
		while ((left + 1) != right)
		{
			size_t middle = (left + right) / 2;
			int cmp = _strcmp(key, length, super::at(middle).key, super::at(middle).keysize);
			if (cmp < 0)		right = middle - 1; 
			else if (cmp == 0)	{ *position = middle; _last = middle; return true; }
			else				left = middle + 1;
		}

		int cmp = _strcmp(key, length, super::at(left).key, super::at(left).keysize);
		if (cmp < 0)			{ *position = left; return false; }
		if (cmp == 0)			{ *position = left; _last = left; return true; }
		else
		{
			cmp = _strcmp(key, length, super::at(right).key, super::at(right).keysize);
			if (cmp < 0)		{ *position = right; return false;	}
			else if (cmp == 0)	{ *position = right; _last = right;	return true; }
			else				{ *position = right + 1; return false; }
		}
	}
}
	
template <class T, class C> ir::QuietRegister<T, C>::QuietRegister() noexcept
{
		
}
	
template <class T, class C> ir::QuietRegister<T, C>::QuietRegister(QuietRegister<T, C> &reg) noexcept
: QuietVector<RegisterElement<T, C>>(reg)
{
		
}
	
template <class T, class C> T &ir::QuietRegister<T, C>::operator[](const C *key) noexcept
{
	return at(key, _strlen(key));
}
	
template <class T, class C> T &ir::QuietRegister<T, C>::at(const C *key) noexcept
{
	return at(key, _strlen(key));
}
	
template <class T, class C> T &ir::QuietRegister<T, C>::at(const C *key, size_t length) noexcept
{
	size_t position;
	if (_find(key, length, &position)) return super::at(position).data;
	else
	{
		RegisterElement<T, C> elem;
		elem.key = _strdup(key, length);
		if (elem.key == nullptr) return _dummy.data;
		elem.keysize = length;
		memset(&elem.data, 0, sizeof(T));
		if (!super::insert(position, elem)) { free(elem.key); return _dummy.data; }
		return super::at(position).data;
	}
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::set(const C *key, T elem) noexcept
{
	return set(key, _strlen(key), elem);
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::set(const C *key, size_t length, T elem) noexcept
{
		
	size_t position;
	if (_find(key, length, &position)) super::at(position).data = elem;
	else
	{
		RegisterElement<T, C> welem;
		welem.key = _strdup(key, length);
		if (welem.key == nullptr) return false;
		welem.keysize = length;
		welem.data = elem;
		if (!super::insert(position, welem)) { free(welem.key); return false; }
	}
	return true;
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::erase(const C *key) noexcept
{
	return erase(key, _strlen(key));
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::erase(const C *key, size_t length) noexcept
{
	size_t position;
	if (_find(key, length, &position))
	{
		free(super::at(position).key);
		return super::erase(position);
	}
	else return false;
}
	
template <class T, class C> T &ir::QuietRegister<T, C>::direct(size_t index) noexcept
{
	return super::at(index).data;
}
	
template <class T, class C> const T &ir::QuietRegister<T, C>::operator[](const C *key) const noexcept
{
	return at(key, _strlen(key));
}

template <class T, class C> const T &ir::QuietRegister<T, C>::at(const C *key) const noexcept
{
	return at(key, _strlen(key));
}

template <class T, class C> const T &ir::QuietRegister<T, C>::at(const C *key, size_t length) const noexcept
{
	size_t position;
	if (_find(key, length, &position)) return super::at(position).data;
	else return _dummy.data;
}

template <class T, class C> bool ir::QuietRegister<T, C>::get(const C *key, T *elem) const noexcept
{
	return get(key, _strlen(key), elem);
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::get(const C *key, size_t length, T *elem) const noexcept
{
	size_t position;
	if (_find(key, length, &position)) { *elem = super::at(position).data; return true; }
	else return false;
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::has(const C *key) const noexcept
{
	return has(key, _strlen(key));
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::has(const C *key, size_t length) const noexcept
{
	size_t position;
	return _find(key, length, &position);
}
	
template <class T, class C> const T &ir::QuietRegister<T, C>::direct(size_t index) const noexcept
{
	return super::at(index).data;
}
	
template <class T, class C> const C *ir::QuietRegister<T, C>::direct_key(size_t index) const noexcept
{
	return super::at(index).key;
}

template <class T, class C> size_t ir::QuietRegister<T, C>::direct_key_length(size_t index) const noexcept
{
	return super::at(index).keysize;
}

template <class T, class C> void ir::QuietRegister<T, C>::clear() noexcept
{
	for (size_t i = 0; i < size(); i++)
	{
		free(super::at(i).key);
	}
	super::clear();
}
	
template <class T, class C> bool ir::QuietRegister<T, C>::detach() noexcept
{
	if (super::detach())
	{
		for (size_t i = 0; i < size(); i++)
		{
			super::at(i).key = _strdup(super::at(i).key, super::at(i).keysize);
			if (super::at(i).key == nullptr) return false;
		}
	}
	return true;
}
	
template <class T, class C> ir::QuietRegister<T, C>::~QuietRegister() noexcept
{
	clear();
}

#endif //#ifndef IR_QUIET_REGISTER_IMPLEMENTATION