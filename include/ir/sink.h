/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_SINK
#define IR_SINK

#include "types.h"

namespace ir
{
///@addtogroup container Containers
///@{
	
	///Class to write data to
	class Sink
	{
	public:
		virtual bool limited()								const noexcept = 0;
		virtual size_t available()							const noexcept = 0;
		virtual size_t write(const void *data, size_t size)	noexcept = 0;
		virtual ~Sink()										noexcept = 0;
	};

	class MemorySink : public Sink
	{
	private:
		void *_memory;
		size_t _size;

	public:
		MemorySink(void *memory, size_t size = (size_t)-1)	noexcept;
		void open(void *memory, size_t size = (size_t)-1)	noexcept;
		virtual bool limited()								const noexcept;
		virtual size_t available()							const noexcept;
		virtual size_t write(const void *data, size_t size)	noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_SINK) : !defined(IR_EXCLUDE_SINK)
	#ifndef IR_INCLUDE
	#elif IR_INCLUDE == 'a'
		#ifndef IR_SINK_SOURCE
			#define IR_SINK_SOURCE
			#include "../../source/sink.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_SINK