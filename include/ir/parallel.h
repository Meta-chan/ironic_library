/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_PARALLEL
#define IR_PARALLEL

#include "types.h"

//IR_PARALLEL_IMPLEMENTATION can be
//nothing for noting
//'o' for OpenMP
//'w' for Windows
//'p' for Posix

#ifndef IR_PARALLEL_IMPLEMENTATION
#elif IR_PARALLEL_IMPLEMENTATION == 'w'
	#include <Windows.h>
#elif IR_PARALLEL_IMPLEMENTATION == 'a'
	#include <atomic>
#elif IR_PARALLEL_IMPLEMENTATION == 'o'
	#include <atomic>
#endif

namespace ir
{
///@addtogroup hiperf Hight performance computing
///@{
	
	///Object to perform parallel operations
	class Parallel
	{
	public:
		///Type of function to be performed
		///@param user Pointer given to `ir::Parallel::parallel`
		///@param id Thread number, from `0` to`n - 1`, where `n - 1` corresponds main thread
		typedef void Function(const void *user, uint32 id, uint32 n);

	private:
		static bool _ok;
		
		#ifndef IR_PARALLEL_IMPLEMENTATION
			static uint32 _n;
		#elif IR_PARALLEL_IMPLEMENTATION == 'w'
			static Function * volatile _function;
			static const void * volatile _user;
			static uint32 _n;
			static volatile uint32 _finished;
			static volatile uint32 _task;
			static DWORD WINAPI _windows_function(LPVOID) noexcept;
		#elif IR_PARALLEL_IMPLEMENTATION == 'p'
			static Function * volatile _function;
			static const void * volatile _user;
			static uint32 _n;
			static volatile std::atomic<uint32> _finished;
			static volatile std::atomic<uint32> _task;
			static void *_posix_function(void *) noexcept;
		#elif IR_PARALLEL_IMPLEMENTATION == 'o'
			static uint32 _n;
		#endif
		
	public:
		///Initializes parallel system
		///@param n Number of threads including main thread
		static bool init(uint32 n)									noexcept;
		///Finalizes parallel system
		static void finalize()										noexcept;
		///Returns if parallel system is ok
		static bool ok()											noexcept;
		///Executes function in parallel, returns after all threads have finished
		///@param user Pointer to pass to the function
		///@param function Function to execute
		static bool parallel(const void *user, Function *function)	noexcept;
	};

///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_PARALLEL) : !defined(IR_EXCLUDE_PARALLEL)
	#ifndef IR_INCLUDE
	#elif IR_INCLUDE == 'a'
		#ifndef IR_PARALLEL_SOURCE
			#define IR_PARALLEL_SOURCE
			#include "../../source/parallel.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_PARALLEL