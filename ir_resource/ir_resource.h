#ifndef IR_RESOURCE
#define IR_RESOURCE

namespace ir
{
	template <class T, class F, T N> class Resource
	{
	public:
		T it;
		Resource();
		Resource(T value);
		void free();
		~Resource();
	};
};

#if (defined(IR_IMPLEMENT) || defined(IR_RESOURCE_IMPLEMENT)) && !defined(IR_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_resource_implementation.h>
#endif

#endif //#ifndef IR_RESOURCE