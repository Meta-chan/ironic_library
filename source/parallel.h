/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

bool ir::Parallel::_ok = false;

#ifndef IR_PARALLEL_IMPLEMENTATION

ir::uint32 ir::Parallel::_n = 0;

bool ir::Parallel::init(uint32 n) noexcept
{
	if (_ok || n == 0) return false;
	_n = n;
	_ok = true;
	return true;
}

void ir::Parallel::finalize() noexcept
{
	_ok = false;
}

bool ir::Parallel::parallel(const void *user, Function *function) noexcept
{
	if (!_ok || function == nullptr) return false;
	for (uint32 i = 0; i < _n; i++)
	{
		function(user, i, _n);
	}
	return true;
}

#elif IR_PARALLEL_IMPLEMENTATION == 'w'

#include <Windows.h>

ir::Parallel::Function * volatile ir::Parallel::_function	= nullptr;
const void * volatile ir::Parallel::_user					= nullptr;
ir::uint32 ir::Parallel::_n									= 0;
volatile ir::uint32 ir::Parallel::_finished					= 0;
volatile ir::uint32 ir::Parallel::_task						= 0;

DWORD WINAPI ir::Parallel::_windows_function(LPVOID) noexcept
{
	uint32 id = InterlockedIncrement(&_finished);
	uint32 task = 1;

	while (true)
	{
		if (task != _task)
		{
			//Wait for next task
		}
		else if (_function == nullptr)
		{
			//Quit
			InterlockedIncrement(&_finished);
			return 0;
		}
		else
		{
			//Execute task
			_function(_user, id, _n);
			InterlockedIncrement(&_finished);
			task++;
		}
	}
}

bool ir::Parallel::init(uint32 n) noexcept
{
	if (_ok || n == 0) return false;
	_function = nullptr;
	_user = nullptr;
	_finished = 0;
	_task = 0;
	_n = n;
	for (uint32 i = 0; i < (n - 1); i++)
	{
		if (CreateThread(nullptr, 0, _windows_function, nullptr, 0, nullptr) == NULL) return false;
	}
	while (_finished != (n - 1)) {}
	_ok = true;
	return true;
}

void ir::Parallel::finalize() noexcept
{
	_function = nullptr;
	_user = nullptr;
	_finished = 0;
	_task++;
	while (_finished != (_n - 1)) {}
	_ok = false;
}

bool ir::Parallel::parallel(const void *user, Function *function) noexcept
{
	if (!_ok || function == nullptr) return false;
	_function = function;
	_user = user;
	_finished = 0;
	_task++;
	function(user, 0, _n);
	while (_finished != (_n - 1)) {}
	return true;
}

#elif IR_PARALLEL_IMPLEMENTATION == 'p'

#include <pthread.h>

ir::Parallel::Function * volatile ir::Parallel::_function	= nullptr;
const void * volatile ir::Parallel::_user					= nullptr;
ir::uint32 ir::Parallel::_n									= 0;
volatile std::atomic<ir::uint32> ir::Parallel::_finished	= 0;
volatile std::atomic<ir::uint32> ir::Parallel::_task		= 0;

void *ir::Parallel::_posix_function(void *) noexcept
{
	uint32 id = _finished.fetch_add(1);
	uint32 task = 1;

	while (true)
	{
		if (task != _task)
		{
			//Wait for next task
		}
		else if (_function == nullptr)
		{
			//Quit
			_finished.fetch_add(1);
			return 0;
		}
		else
		{
			//Execute task
			_function(_user, id, _n);
			_finished.fetch_add(1);
			task++;
		}
	}
}

bool ir::Parallel::init(uint32 n) noexcept
{
	if (_ok || n == 0) return false;
	_function = nullptr;
	_user = nullptr;
	_finished = 0;
	_task = 0;
	_n = n;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for (uint32 i = 0; i < (n - 1); i++)
	{
		if (pthread_create(nullptr, &attr, _posix_function, this) != 0) return false;
	}
	pthread_attr_destroy(&attr);
	while (_finished != (n - 1)) {}
	_ok = true;
	return true;
}

void ir::Parallel::finalize() noexcept
{
	_function = nullptr;
	_user = nullptr;
	_finished = 0;
	_task++;
	while (_finished != (_n - 1)) {}
	_ok = false;
}

bool ir::Parallel::parallel(const void *user, Function *function) noexcept
{
	if (!_ok || function == nullptr) return false;
	_function = function;
	_user = user;
	_finished = 0;
	_task++;
	function(user, 0, _n);
	while (_finished != (_n - 1)) {}
	return true;
}

#elif IR_PARALLEL_IMPLEMENTATION == 'o'

#include <omp.h>

ir::uint32 ir::Parallel::_n								= 0;

bool ir::Parallel::init(uint32 n) noexcept
{
	if (_ok || n == 0) return false;
	_n = n;
	_ok = true;
	return true;
}

void ir::Parallel::finalize() noexcept
{
	_ok = false;
}

bool ir::Parallel::parallel(const void *user, Function *function) noexcept
{
	if (!_ok || function == nullptr) return false;
	#pragma omp parallel num_threads(_n)
	{
		function(user, omp_get_thread_num(), _n);
		#pragma omp barrier
	}
	return true;
}

#endif

bool ir::Parallel::ok() noexcept
{
	return _ok;
}