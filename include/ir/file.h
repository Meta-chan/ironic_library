/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_FILE
#define IR_FILE

#include "types.h"
#include <stddef.h>
#include <stdio.h>

namespace ir
{
///@addtogroup file File utilities
///@{
	
	class File
	{
	private:
		struct Header
		{
			size_t refcount;
			uint64 position;
			FILE *file;
		};

		Header *_header = nullptr;
	
	public:
		File()												noexcept;
		File(const schar *filename, const schar *mode)		noexcept;
		File(const File &file)								noexcept;
		const File &assign(const File &file)				noexcept;
		const File &operator=(const File &file)				noexcept;
		bool open(const schar *filename, const schar *mode)	noexcept;
		void close()										noexcept;
		bool seek(uint64 position, int mode)				noexcept;
		uint64 tell()										const noexcept;
		uint64 size()										noexcept;
		bool ok()											const noexcept;
		FILE *file()										noexcept;
		size_t read(void *data, size_t size)				noexcept;
		size_t write(const void *data, size_t size)			noexcept;
		~File()												noexcept;
	};
	
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_FILE) : !defined(IR_EXCLUDE_FILE)
	#ifndef IR_INCLUDE
		
	#elif IR_INCLUDE == 'a'
		#ifndef IR_FILE_SOURCE
			#define IR_FILE_SOURCE
			#include "../../source/file.h"
		#endif
	#endif
#endif

#endif	//#ifndef IR_FILE