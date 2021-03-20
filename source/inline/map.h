/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <stdexcept>

template<class K, class V, class C>
inline V &ir::Map<K, V, C>::operator[](const K &key)
{
	return at(key);
}

template<class K, class V, class C>
inline V &ir::Map<K, V, C>::at(const K &key)
{
	size_t position;
	assert(super::_find(key, &position));
	if (super::super::_header->refcount > 1)
	{
		if (!super::super::_detach()) throw std::bad_alloc();
	}
	return super::super::at(position).value;
}

template<class K, class V, class C>
inline void ir::Map<K, V, C>::set(const K &key, const V &elem)
{
	if (!super::set(key, elem)) throw std::bad_alloc();
}

template<class K, class V, class C>
inline void ir::Map<K, V, C>::erase(const K &key)
{
	if (!super::erase(key)) throw std::bad_alloc();
}

template<class K, class V, class C>
inline V &ir::Map<K, V, C>::direct_value(size_t index)
{
	if (super::super::_header != nullptr && super::super::_header->refcount > 1)
	{
		if (super::super::_detach()) throw std::bad_alloc();
	}
	return super::super::at(index).value;
}

template<class K, class V, class C>
inline const V &ir::Map<K, V, C>::operator[](const K &key) const noexcept
{
	return at(key);
}

template<class K, class V, class C>
inline const V &ir::Map<K, V, C>::at(const K &key) const noexcept
{
	return super::at(key);
}

template<class K, class V, class C>
inline const V &ir::Map<K, V, C>::direct_value(size_t index) const noexcept
{
	return super::direct_value(index);
}