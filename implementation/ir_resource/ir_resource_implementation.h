/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RESOURCE_IMPLEMENTATION
#define IR_RESOURCE_IMPLEMENTATION

template <class T, class IniterFreer> ir::Resource<T, IniterFreer>::Resource() : _value(IniterFreer::initvalue)
{
}

template <class T, class IniterFreer> ir::Resource<T, IniterFreer>::Resource(const T &value) : _value(value)
{
}

template <class T, class IniterFreer> T &ir::Resource<T, IniterFreer>::value()
{
	return _value;
}

template <class T, class IniterFreer> const T &ir::Resource<T, IniterFreer>::value() const
{
	return _value;
}

template <class T, class IniterFreer> ir::Resource<T, IniterFreer>::operator T() const
{
	return _value;
}

template <class T, class IniterFreer> T &ir::Resource<T, IniterFreer>::operator=(const T &value)
{
	_value = value;
	return _value;
}

template <class T, class IniterFreer> void ir::Resource<T, IniterFreer>::free()
{
	IniterFreer::free(_value);
	_value = IniterFreer::initvalue;
}

template <class T, class IniterFreer> ir::Resource<T, IniterFreer>::~Resource()
{
	IniterFreer::free(_value);
	_value = IniterFreer::initvalue;
}

#endif	//#ifndef IR_RESOURCE_IMPLEMENTATION