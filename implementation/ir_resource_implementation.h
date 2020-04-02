#ifndef IR_RESOURCE_IMPLEMENTATION
#define IR_RESOURCE_IMPLEMENTATION

template <class T, class F, T N> ir::Resource<T, F, N>::Resource()
{
	it = N;
};

template <class T, class F, T N> ir::Resource<T, F, N>::Resource(T value) : it(value)
{
};

template <class T, class F, T N> void ir::Resource<T, F, N>::free()
{
	if (it != N)
	{
		F::free(it);
		it = N;
	}
};

template <class T, class F, T N> ir::Resource<T, F, N>::~Resource()
{
	free();
};

#endif	//#ifndef IR_RESOURCE_IMPLEMENTATION