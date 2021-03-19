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
bool ir::QuietMap<K, V, C>::_find(const K &key, size_t *position) const noexcept
{
	if (size() == 0)	{ *position = 0; return false;	}
	else if (size() == 1)
	{
		int cmp = C::compare(key, super::at(0).key);
		if (cmp < 0)	{ *position = 0; return false; }
		if (cmp == 0)	{ *position = 0; _last = 0; return true; }
		else			{ *position = 1; return false; }
	}
	else if (_last < size() && C::compare(key, super::at(_last).key) == 0)
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
			int cmp = C::compare(key, super::at(middle).key);
			if (cmp < 0)		right = middle - 1; 
			else if (cmp == 0)	{ *position = middle; _last = middle; return true; }
			else				left = middle + 1;
		}

		int cmp = C::compare(key, super::at(left).key);
		if (cmp < 0)			{ *position = left; return false; }
		if (cmp == 0)			{ *position = left; _last = left; return true; }
		else
		{
			cmp = C::compare(key, super::at(right).key);
			if (cmp < 0)		{ *position = right; return false;	}
			else if (cmp == 0)	{ *position = right; _last = right;	return true; }
			else				{ *position = right + 1; return false; }
		}
	}
}

template<class K, class V, class C>
ir::QuietMap<K, V, C>::QuietMap() noexcept
{
}

template<class K, class V, class C>
ir::QuietMap<K, V, C>::QuietMap(const QuietMap<K, V, C> &map) noexcept
: QuietVector<MapElement<K, V>>(map)
{		
}

template<class K, class V, class C>
const ir::QuietMap<K, V, C> &ir::QuietMap<K, V, C>::assign(const QuietMap<K, V, C> &map) noexcept
{
	super::assign(map);
	return *this;
}

template<class K, class V, class C>
const ir::QuietMap<K, V, C> &ir::QuietMap<K, V, C>::operator=(const QuietMap<K, V, C> &map) noexcept
{
	super::assign(map);
	return *this;
}