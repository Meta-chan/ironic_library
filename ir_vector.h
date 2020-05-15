#ifndef IR_VECTOR
#define IR_VECTOR
	
namespace ir
{
	template <class T> class Vector
	{
	protected:
		struct Header
		{
			size_t size		= 0;
			size_t capacity = 0;
			size_t refcount	= 0;
		};
		Header *_header = nullptr;
		static char const * const _out_of_range_message;

	public:
		Vector()							noexcept;
		Vector(size_t newsize);
		Vector(Vector &vector)				noexcept;
		T *data()							noexcept;
		T &operator[](size_t i);
		T &at(size_t i);
		T &front();
		T &back();
		const T *data()						const noexcept;
		const T &operator[](size_t i)		const;
		const T &at(size_t i)				const;
		const T &front()					const;
		const T &back()						const;
		bool empty()						const noexcept;
		size_t size()						const noexcept;
		size_t capacity()					const noexcept;
		void resize(size_t newsize);
		void reserve(size_t newcapacity);
		void push_back(T elem);
		void pop_back();
		void insert(size_t i, T elem);
		void erase(size_t i);
		void clear()						noexcept;
		void detach();
		~Vector()							noexcept;
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_VECTOR_IMPLEMENT)) && !defined(IR_VECTOR_NOT_IMPLEMENT)
	#include <implementation/ir_vector_implementation.h>
#endif

#endif //#ifndef IR_VECTOR