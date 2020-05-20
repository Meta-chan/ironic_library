#ifndef IR_RESOURCE
#define IR_RESOURCE

namespace ir
{
	template <class T, class IniterFreer> class Resource
	{
	private:
		T _value;

	public:
		Resource();
		Resource(const T& value);
		T &value();
		const T &value() const;
		operator T() const;
		T &operator=(const T &value);
		void free();
		~Resource();
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_RESOURCE_IMPLEMENT)) && !defined(IR_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_resource_implementation.h>
#endif

#endif //#ifndef IR_RESOURCE