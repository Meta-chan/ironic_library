/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RESOURCE
#define IR_RESOURCE
	
namespace ir
{
///@addtogroup resource RAII
///@{

	///Class wrapper around non-class things, ironic library's implementation of RAII idea. Used to avoid writing lots of freeing functions.
	///@tparam T Type to be transformed to class
	template <class T> class Resource
	{
	public:
		typedef void FreeFunction(T &value);
	
	private:
		T _value;
		FreeFunction *_freefunction;
		
	public:
		///Create resource with invalid value which does not need to be freed
		///@param freefunction Function needs to be called to free resource
		Resource(FreeFunction *freefunction)	noexcept;
		///Create resource from other resource
		Resource(const Resource &other)			noexcept;
		///Returns reference to stored value
		T &value()								noexcept;
		///Returns constant reference to stored value
		const T &value()						const noexcept;
		///Cast operator to specified type
		operator T()							const noexcept;
		///Assigns stored value to given value
		///@param value Value to be assigned to
		T &operator=(const T &value)			noexcept;
		///Calls freeing function
		void free()								noexcept;
		///Destroys resource and calls freeing function
		~Resource()								noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_RESOURCE) : !defined(IR_EXCLUDE_RESOURCE)
	#ifndef IR_INCLUDE
		#ifndef IR_RESOURCE_TEMPLATE_SOURCE
			#define IR_RESOURCE_TEMPLATE_SOURCE
			#include "../../source/template/resource.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_RESOURCE_TEMPLATE_SOURCE
			#define IR_RESOURCE_TEMPLATE_SOURCE
			#include "../../source/template/resource.h"
		#endif
	#endif
#endif

#endif //#ifndef IR_RESOURCE