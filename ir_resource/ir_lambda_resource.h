#ifndef IR_LAMBDA_RESOURCE
#define IR_LAMBDA_RESOURCE
	
namespace ir
{
///@defgroup ir_resource Resources
///@{

	///Lambda resource is a lite version of LambdaResource. It does not need to define IniterFreer, but each instance stores pointer to freeing function.
	///@tparam T Type to be transformed to class with class
	template <class T> class LambdaResource
	{
	public:
		typedef void FreeFunction(T *value);
	
	private:
		T _value;
		FreeFunction *_freefunction;
		
	public:
		///Create resource with invalid value which does not need to be freed
		///@param freefunction Function needs to be called to free resource
		LambdaResource(FreeFunction *freefunction);
		///Create resource from other resource
		LambdaResource(const LambdaResource &other);
		///Returns reference to stored value
		T &value();
		///Returns constant reference to stored value
		const T &value() const;
		///Cast operator to specified type
		operator T() const;
		///Assings stored value to given value
		///@param value Value to be assigned to
		T &operator=(const T &value);
		///Calls freeing function
		void free();
		///Destroyes resource and calls freeing function
		~LambdaResource();
	};
	
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_LAMBDA_RESOURCE_IMPLEMENT)) && !defined(IR_LAMBDA_RESOURCE_NOT_IMPLEMENT)
	#include <implementation/ir_resource/ir_lambda_resource_implementation.h>
#endif

#endif //#ifndef IR_LAMBDA_RESOURCE