/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_MAP_IMPLEMENTATION
#define IR_MAP_IMPLEMENTATION

#include <stdexcept>

template<class K, class V, class C> ir::Map<K, V, C>::Map() noexcept
{}

template<class K, class V, class C> ir::Map<K, V, C>::Map(const QuietMap<K, V, C> &map) noexcept : QuietMap<K, V, C>::QuietMap(map)
{}

template<class K, class V, class C> V &ir::Map<K, V, C>::operator[](const K &key)
{
	return at(key);
}

template<class K, class V, class C> V &ir::Map<K, V, C>::at(const K &key)
{
	if (!detach()) throw std::bad_alloc();
	return super::at(key);
}

template<class K, class V, class C> void ir::Map<K, V, C>::set(const K &key, const V &elem)
{
	if (!detach() || !super::set(key, elem)) throw std::bad_alloc();
}

template<class K, class V, class C> void ir::Map<K, V, C>::erase(const K &key)
{
	if (!detach()) throw std::bad_alloc();
	super::erase(key);
}

template<class K, class V, class C> V &ir::Map<K, V, C>::direct_value(size_t index)
{
	if (!detach()) throw std::bad_alloc();
	return super::direct_value(index);
}

template<class K, class V, class C> const V &ir::Map<K, V, C>::operator[](const K &key) const noexcept
{
	return at(key);
}

template<class K, class V, class C> const V &ir::Map<K, V, C>::at(const K &key) const noexcept
{
	return super::at(key);
}

template<class K, class V, class C> const V &ir::Map<K, V, C>::direct_value(size_t index) const noexcept
{
	return super::direct_value(index);
}

#endif //#ifndef IR_MAP_IMPLEMENTATION