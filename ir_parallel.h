/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_PARALLEL
#define IR_PARALLEL

#ifdef _WIN32
	#include <Windows.h>
#endif

namespace ir
{
///@defgroup ir_parralel Parallel operations
///@{
	
	///Object to perform parallel operations
	class Parallel
	{
	public:
		///Type of function to be performed
		///@param user Pointer given to <tt>ir::Parallel::parallel</tt>
		///@param id Thread number, from <tt>0</tt> to <tt>n - 1</tt>, where <tt>n - 1</tt> corresponds main thread
		typedef void Function(const void *user, unsigned int id);

	private:

		volatile Function *_function	= nullptr;
		volatile const void *_user		= nullptr;
		unsigned int _n					= 0;
		unsigned int _timeout			= 0;
		volatile unsigned int _finished	= 0;
		volatile unsigned int _task		= 0;
		bool _ok						= false;
		
		static inline unsigned int _increment(volatile unsigned int *i);
		#ifdef _WIN32
			static DWORD WINAPI _internal_function(LPVOID paralle);
		#else
			static void *_internal_function(void *paralle);
		#endif
		
	public:
		///Creates Parallel object
		///@param n Number of threads including main thread
		///@param timeout After @c timeout milliseconds threads will no longer actively check for new tasks, but will sleep. It releases system resources, but takes longer to wake the threads up
		Parallel(unsigned int n, unsigned int timeout);
		///Checks if object is ok
		bool ok() const;
		///Executes function in parallel, returns after all threads have finished
		///@param user Pointer to pass to the function
		///@param function Function to execute
		bool parallel(const void *user, Function *function);
		///Destroys parallel object, releases all threads
		~Parallel();
	};

///@}
};

#if (defined(IR_IMPLEMENT) || defined(IR_PARALLEL_IMPLEMENT)) && !defined(IR_PARALLEL_NOT_IMPLEMENT)
	#include <implementation/ir_parallel_implementation.h>
#endif

#endif	//#ifndef IR_PARALLEL