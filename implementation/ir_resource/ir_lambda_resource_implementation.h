/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_LAMBDA_RESOURCE_IMPLEMENTATION
#define IR_LAMBDA_RESOURCE_IMPLEMENTATION

template <class T> ir::LambdaResource<T>::LambdaResource(FreeFunction *freefunction) : _freefunction(freefunction)
{
};

template <class T> ir::LambdaResource<T>::LambdaResource(const LambdaResource &other) : _value(other._value), _freefunction(other._freefunction)
{
};

template <class T> T &ir::LambdaResource<T>::value()
{
	return _value;
};

template <class T> const T &ir::LambdaResource<T>::value() const
{
	return _value;
};

template <class T> ir::LambdaResource<T>::operator T() const
{
	return _value;
};

template <class T> T &ir::LambdaResource<T>::operator=(const T &value)
{
	_value = value;
	return _value;
};

template <class T> void ir::LambdaResource<T>::free()
{
	_freefunction(&_value);
};

template <class T> ir::LambdaResource<T>::~LambdaResource()
{
	_freefunction(&_value);
};

#endif	//#ifndef IR_LAMBDA_RESOURCE_IMPLEMENTATION