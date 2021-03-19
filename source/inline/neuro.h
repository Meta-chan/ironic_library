/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <math.h>

template<class T>
inline T ir::Tanh<T>::function(const T input) noexcept
{
	return tanh(input);
}

template<class T>
inline T ir::Tanh<T>::derivative(const T output) noexcept
{
	return 1 - output * output;
}

template<class T>
inline T ir::ReLU<T>::function(const T input) noexcept
{
	return input >= 0 ? input : 0.01 * input;
}

template<class T>
inline T ir::ReLU<T>::derivative(const T output) noexcept
{
	return output >= 0 ? 1.0 : 0.01;
}