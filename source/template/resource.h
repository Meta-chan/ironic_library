/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template <class T> ir::Resource<T>::Resource(FreeFunction *freefunction) noexcept : _freefunction(freefunction)
{
}

template <class T> ir::Resource<T>::Resource(const Resource &other) noexcept : _value(other._value), _freefunction(other._freefunction)
{
}

template <class T> T &ir::Resource<T>::value() noexcept
{
	return _value;
}

template <class T> const T &ir::Resource<T>::value() const noexcept
{
	return _value;
}

template <class T> ir::Resource<T>::operator T() const noexcept
{
	return _value;
}

template <class T> T &ir::Resource<T>::operator=(const T &value) noexcept
{
	_value = value;
	return _value;
}

template <class T> void ir::Resource<T>::free() noexcept
{
	_freefunction(_value);
}

template <class T> ir::Resource<T>::~Resource() noexcept
{
	_freefunction(_value);
}