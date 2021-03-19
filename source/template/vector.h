/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

template<class T>
ir::Vector<T>::Vector() noexcept
{
}

template<class T>
ir::Vector<T>::Vector(size_t newsize)
{
	resize(newsize);
}

template<class T>
ir::Vector<T>::Vector(const QuietVector<T> &vector) noexcept : QuietVector<T>(vector)
{
}