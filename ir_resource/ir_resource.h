#ifndef IR_RESOURCE
#define IR_RESOURCE
	
namespace ir
{
///@defgroup ir_resource Resources
///@{

	///Resource. Class wrapper around non-class things. Ironic library's implementation of RAII idea
	///Acts almost like specified type
	///@tparam T Type to be transformed to class with class
	///@tparam IniterFreer Helper class with initvalue and free methods
	template <class T, class IniterFreer> class Resource
	{
	private:
		T _value;

	public:
		///Create resource with invalid value which does not need to be freed
		Resource();
		///Create resource from other resource
		Resource(const T& value);
		///Returns reference to stored value
		T &value();
		///Returns constant reference to stored value
		const T &value() const;
		///Cast operator to specified type
		operator T() const;
		///Assings stored value to given value
		///@param value Value to be assigned to
		T &operator=(const T &value);
		///Frees the resource if stored value is valid it and invalidates it
		void free();
		///Destroyes resource and frees it if stored value is valid
		~Resource();
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_RESOURCE_IMPLEMENT)) && !defined(IR_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_resource_implementation.h>
#endif

#endif //#ifndef IR_RESOURCE