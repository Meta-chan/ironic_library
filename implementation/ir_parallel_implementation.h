/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_PARALLEL_IMPLEMENTATION
#define IR_PARALLEL_IMPLEMENTATION

#ifndef _WIN32
	#include <pthread.h>
	#ifndef __GNUC__
		#include <atomic>
	#endif
#endif

unsigned int ir::Parallel::_increment(volatile unsigned int *i)
{
	#if defined(_WIN32)
		return InterlockedIncrement(i);
	#elif defined(__GNUC__)
		return __atomic_add_fetch(i, 1, __ATOMIC_SEQ_CST);
	#else
		return std::atomic<unsigned int>::fetch_and(i, 1);
	#endif
};

#ifdef _WIN32
	DWORD WINAPI ir::Parallel::_internal_function(LPVOID paralle)
#else
	void *ir::Parallel::_internal_function(void *paralle)
#endif
{
	Parallel *parallel = (Parallel*)paralle;
	unsigned int id = _increment(&parallel->_finished);
	unsigned int task = 1;

	//AAAAH, it needs to sleeeeeep!
	while (true)
	{
		if (task != parallel->_task) {}
		else if (parallel->_function == nullptr)
		{
			_increment(&parallel->_finished);
			#ifdef _WIN32
				return 0;
			#else
				return nullptr;
			#endif
		}
		else
		{
			parallel->_function(parallel->_user, id);
			_increment(&parallel->_finished);
			task++;
		}
	}
};

ir::Parallel::Parallel(unsigned int n, unsigned int timeout)
{
	if (n == 0) return;
	_n = 1;
	_timeout = timeout;
	#ifdef _WIN32
		for (unsigned int i = 0; i < (n - 1); i++)
		{
			if (CreateThread(nullptr, 0, _internal_function, this, 0, nullptr) != NULL) _n++;
			else break;
		}
	#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		for (unsigned int i = 0; i < (n - 1); i++)
		{
			if (pthread_create(nullptr, &attr, _internal_function, this) == 0) _n++;
			else break;
		}
		pthread_attr_destroy(&attr);
	#endif
	while (_finished != (_n - 1)) {}
	if (_n == n) _ok = true;
};

bool ir::Parallel::ok() const
{
	return _ok;
};

bool ir::Parallel::parallel(const void *user, Function *function)
{
	if (!_ok) return false;
	if (function == nullptr) return false;
	_function = function;
	_user = user;
	_finished = 0;
	_task++;
	function(user, 0);
	while (_finished != (_n - 1)) {}
	return true;
};

ir::Parallel::~Parallel()
{
	_function = nullptr;
	_user = nullptr;
	_finished = 0;
	_task++;
	while (_finished != (_n - 1)) {}
	_ok = false;
};

#endif //#ifndef IR_PARALLEL_IMPLEMENTATION