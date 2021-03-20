/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template<class K, class V, class C>
inline V &ir::QuietMap<K, V, C>::at(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	return super::at(position).value;
}
	
template<class K, class V, class C>
inline V &ir::QuietMap<K, V, C>::operator[](const K &key) noexcept
{
	return at(key);
}
	
template<class K, class V, class C>
inline bool ir::QuietMap<K, V, C>::set(const K &key, const V &elem) noexcept
{		
	size_t position;
	if (QuietVector<MapElement<K, V>>::_header != nullptr && QuietVector<MapElement<K, V>>::_header->refcount > 1)
	{
		if (!super::_detach()) return false;
	}

	if (_find(key, &position))
	{
		super::at(position).value = elem;
		return true;
	}
	else
	{
		MapElement<K, V> welem;
		welem.key = key;
		welem.value = elem;
		return super::insert(position, welem);
	}
}
	
template<class K, class V, class C>
inline bool ir::QuietMap<K, V, C>::erase(const K &key) noexcept
{
	size_t position;
	assert(_find(key, &position));
	return super::erase(position);
}
	
template<class K, class V, class C>
inline V &ir::QuietMap<K, V, C>::direct_value(size_t index) noexcept
{
	return super::at(index).value;
}
	
template<class K, class V, class C>
inline const V &ir::QuietMap<K, V, C>::at(const K &key) const noexcept
{
	size_t position;
	assert(_find(key, &position));
	return super::at(position).value;
}

template<class K, class V, class C>
inline const V &ir::QuietMap<K, V, C>::operator[](const K &key) const noexcept
{
	return at(key);
}

template<class K, class V, class C>
inline bool ir::QuietMap<K, V, C>::has(const K &key) const noexcept
{
	size_t position;
	return _find(key, &position);
}
	
template<class K, class V, class C>
inline const V &ir::QuietMap<K, V, C>::direct_value(size_t index) const noexcept
{
	return super::at(index).value;
}
	
template<class K, class V, class C>
inline const K &ir::QuietMap<K, V, C>::direct_key(size_t index) const noexcept
{
	return super::at(index).key;
}