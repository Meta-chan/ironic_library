/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_QUIET_VECTOR
#define IR_QUIET_VECTOR

#include <stddef.h>

namespace ir
{
	template <class T> class QuietVector
	{
	protected:
		struct Header
		{
			size_t size		= 0;
			size_t capacity = 0;
			size_t refcount	= 0;
		};
		Header *_header = nullptr;
		static T _dummy;

	public:
		QuietVector()						noexcept;
		QuietVector(size_t newsize)			noexcept;
		QuietVector(QuietVector &vector)	noexcept;

		T *data()							noexcept;
		T &operator[](size_t i)				noexcept;
		T &at(size_t i)						noexcept;
		T &front()							noexcept;
		T &back()							noexcept;
		bool set(size_t i, T elem)			noexcept;
		
		const T *data()						const noexcept;
		const T &operator[](size_t i)		const noexcept;
		const T &at(size_t i)				const noexcept;
		const T &front()					const noexcept;
		const T &back()						const noexcept;
		bool get(size_t i, T *elem)			const noexcept;

		bool empty()						const noexcept;
		size_t size()						const noexcept;
		size_t capacity()					const noexcept;
		bool resize(size_t newsize)			noexcept;
		bool reserve(size_t newcapacity)	noexcept;
		bool push_back(T elem)				noexcept;
		bool pop_back()						noexcept;
		bool insert(size_t i, T elem)		noexcept;
		bool erase(size_t i)				noexcept;
		void clear()						noexcept;
		bool detach()						noexcept;

		~QuietVector()							noexcept;
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_QUIET_VECTOR_IMPLEMENT)) && !defined(IR_QUIET_VECTOR_NOT_IMPLEMENT)
	#include <implementation/ir_container/ir_quiet_vector_implementation.h>
#endif

#endif //#ifndef IR_QUIET_VECTOR