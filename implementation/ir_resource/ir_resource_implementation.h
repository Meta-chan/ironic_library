/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RESOURCE_IMPLEMENTATION
#define IR_RESOURCE_IMPLEMENTATION

template <class T, class F, T N> ir::Resource<T, F, N>::Resource() : it(N)
{
};

template <class T, class F, T N> ir::Resource<T, F, N>::Resource(T value) : it(value)
{
};

template <class T, class F, T N> void ir::Resource<T, F, N>::free()
{
	F::free(it);
	it = N;
};

template <class T, class F, T N> ir::Resource<T, F, N>::~Resource()
{
	F::free(it);
	it = N;
};

#endif	//#ifndef IR_RESOURCE_IMPLEMENTATION