/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#include <assert.h>
#include <string.h>
#include <wchar.h>

template <class T>
inline T *ir::String<T>::data() noexcept
{
	_detach();
	if (_s.mode == Mode::stack) return _s.data;
	else return (T*)(_h.data + 1);
}

template <class T>
inline T &ir::String<T>::at(size_t i)
{
	assert(i < size());
	_detach();
	return data()[i];
}

template <class T>
inline T &ir::String<T>::operator[](size_t i)
{
	assert(i < size());
	_detach();
	return at(i);
}

template <class T>
inline T &ir::String<T>::front()
{
	assert(size() > 0);
	_detach();
	return at(0);
}

template <class T>
inline T &ir::String<T>::back()
{
	assert(size() > 0);
	_detach();
	return at(size() - 1);
}

template <class T>
inline void ir::String<T>::insert(size_t i, T c)
{
	assert(i <= size());
	resize(size() + 1);
	memcmp(data() + i, data() + i + 1, size() - i);
	at(i) = c;
}

template <class T>
inline void ir::String<T>::erase(size_t i)
{
	assert(i < size());
	resize(size() - 1);
	memcmp(data() + i + 1, data() + i, size() - i - 1);
}

template <class T>
inline void ir::String<T>::push_back(T c)
{
	resize(size() + 1);
	back() = c;
}

template <class T>
inline void ir::String<T>::pop_back()
{
	resize(size() - 1);
}

template <class T>
inline const T *ir::String<T>::data() const noexcept
{
	if (_s.mode == Mode::stack) return _s.data;
	else if (_s.mode == Mode::heap) return _h.data == nullptr ? nullptr : (T*)(_h.data + 1);
	else return _c.data;
}

template <class T>
inline const T &ir::String<T>::at(size_t i) const
{
	assert(i < size());
	return at(i);
}

template <class T>
inline const T &ir::String<T>::operator[](size_t i) const
{
	assert(i < size());
	return at(i);
}

template <class T>
inline const T &ir::String<T>::front() const
{
	assert(size() > 0);
	return at(0);
}

template <class T>
inline const T &ir::String<T>::back() const
{
	assert(size() > 0);
	return at(size() - 1);
}

template <class T>
inline bool ir::String<T>::empty() const noexcept
{
	return (size() == 0);
}

template <class T>
inline size_t ir::String<T>::size() const noexcept
{
	if (_s.mode == Mode::stack) return _s.size;
	else if (_h.mode == Mode::heap) return _h.size;
	else if (_c.size == Constant::invalid_size)
	{
		if (sizeof(T) == 1) return _c.size = strlen((char*)_c.data);
		else if (sizeof(T) == 2) return _c.size = wcslen((wchar_t*)_c.data);
		else for (size_t i = 0; true; i++) { if (_c.data[i] == '\0') return _c.size = i; }
	}
	else return _c.size;
}

template <class T>
inline size_t ir::String<T>::capacity() const noexcept
{
	if (_s.mode == Mode::stack) return Stack::max_size - 1;
	else if (_h.mode == Mode::heap) return _h.data->reserved;
	else return 0;
}

template<class T>
inline int ir::compare(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b));
}

template<class T>
inline int ir::compare(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b);
}

template<class T>
inline bool ir::operator==(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) == 0;
}

template<class T>
inline bool ir::operator==(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) == 0;
}

template<class T>
inline bool ir::operator==(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) == 0;
}

template<class T>
inline bool ir::operator!=(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) != 0;
}

template<class T>
inline bool ir::operator!=(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) != 0;
}

template<class T>
inline bool ir::operator!=(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) != 0;
}

template<class T>
inline bool ir::operator<(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) < 0;
}

template<class T>
inline bool ir::operator<(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) < 0;
}

template<class T>
inline bool ir::operator<(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) < 0;
}

template<class T>
inline bool ir::operator<=(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) <= 0;
}

template<class T>
inline bool ir::operator<=(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) <= 0;
}

template<class T>
inline bool ir::operator<=(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) <= 0;
}

template<class T>
inline bool ir::operator>(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) > 0;
}

template<class T>
inline bool ir::operator>(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) > 0;
}

template<class T>
inline bool ir::operator>(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) > 0;
}

template<class T>
inline bool ir::operator>=(const String<T> &a, const String<T> &b) noexcept
{
	return compare(a, b) >= 0;
}

template<class T>
inline bool ir::operator>=(const String<T> &a, const T *b) noexcept
{
	return compare(a, String<T>(b)) >= 0;
}

template<class T>
inline bool ir::operator>=(const T *a, const String<T> &b) noexcept
{
	return compare(String<T>(a), b) >= 0;
}

template<class T>
inline const ir::String<T> &ir::operator+(const String<T> &a, const T *b)
{
	return a + String<T>(b);
}

template<class T>
inline const ir::String<T> &ir::operator+(const T *a, const String<T> &b)
{
	return String<T>(a) + b;
}