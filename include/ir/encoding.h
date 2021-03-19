/*
	Part of the Ironic Project. Distributed under MIT License, which means:
		- Do whatever you want
		- Keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Kyrylo Sovailo, github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_ENCODING
#define IR_ENCODING

#include "types.h"
#include "quiet_vector.h"

namespace ir
{
///@addtogroup encoding Text encoding functions
///@{
	class Encoding
	{
	private:
		static QuietVector<char> _buffer;
	
	public:
		///ASCII codec. See this codec for getting documentation for all other codecs
		class ASCII
		{
		public:
			///Encoding's native character
			typedef char8 Char;

			///Converts the Unicode code to the encoding
			///@param		code		Unicode code
			///@param		string		String in ASCII encoding. May be `nullptr`, then nothing is written
			///@param		nnative		Pointer to `size_t` that receives number of ASCII characters were written. May be `nullptr`
			///@return					`true` if `code` is representable in ASCII encoding
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;

			///Converts specified encoding to one Unicode code
			///@param[in]	string		String in ASCII encoding
			///@param[out]	nnative		Pointer to `size_t` that receives number of ASCII characters were read. May be `nullptr`, then nothing is written
			///@return					Unicode code
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF8 codec
		class UTF8
		{
		public:
			typedef char8 Char;
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF16 codec
		class UTF16
		{
		public:
			typedef char16 Char;
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF32 codec
		class UTF32
		{
		public:
			typedef char32 Char;
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///Windows-1251 (cyrillic) codec
		class CP1251
		{
		public:
			typedef char8 Char;
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
			
		private:
			static const uint16 _decode_table[0x40];
		};
		
		///DOS-866 (cyrillic) codec
		class CP866
		{			
		public:
			typedef char8 Char;
			inline static bool encode(uint32 code, Char *string, size_t *nnative) noexcept;
			inline static uint32 decode(const Char *string, size_t *nnative) noexcept;
			
		private:
			static const uint16 _decode_table1[0x30];
			static const uint16 _decode_table2[0x10];
		};
		
		///Decodes null-terminated source string into null-terminated destination string (null-character is written)
		///Replaces the symbols not representable in destination encoding with @c errcode
		///@tparam	SourceCodec	Source codec
		///@tparam	DestCodec	Destination codec
		///@param	source		Source string with encoding specified with @c codec1
		///@param	errstring	Sequence which is inserted if some symbol is not representable in destination encoding
		///@param	string2		Destination string. May be `nullptr`, then nothing is written
		///@return				Number of native characters of destination string (excluding null character)
		template<class SourceCodec, class DestCodec>
		static size_t recode(typename DestCodec::Char *dest, const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring = (typename DestCodec::Char*)nullptr) noexcept;

		///Same as `recode`, but returns a string, allocated with standard C `malloc`
		template<class SourceCodec, class DestCodec>
		static typename DestCodec::Char *alloc_recode(const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring = (typename DestCodec::Char*)nullptr) noexcept;

		///Same as `recode`, but returns a pointer to internal buffer. Be careful, there is only one internal buffer!
		template<class SourceCodec, class DestCodec>
		static typename DestCodec::Char *buffer_recode(const typename SourceCodec::Char *source, const typename DestCodec::Char *errstring = (typename DestCodec::Char*)nullptr) noexcept;
		
		///Clear internal buffer
		static void clear() noexcept;
	};
///@}
}

#if defined(IR_EXCLUDE) ? defined(IR_INCLUDE_ENCODING) : !defined(IR_EXCLUDE_ENCODING)
	#ifndef IR_INCLUDE
		#ifndef IR_ENCODING_INLINE_SOURCE
			#define IR_ENCODING_INLINE_SOURCE
			#include "../../source/inline/encoding.h"
		#endif
		#ifndef IR_ENCODING_TEMPLATE_SOURCE
			#define IR_ENCODING_TEMPLATE_SOURCE
			#include "../../source/template/encoding.h"
		#endif
	#elif IR_INCLUDE == 'i'
		#ifndef IR_ENCODING_INLINE_SOURCE
			#define IR_ENCODING_INLINE_SOURCE
			#include "../../source/inline/encoding.h"
		#endif
	#elif IR_INCLUDE == 't' || IR_INCLUDE == 'a'
		#ifndef IR_ENCODING_INLINE_SOURCE
			#define IR_ENCODING_INLINE_SOURCE
			#include "../../source/inline/encoding.h"
		#endif
		#ifndef IR_ENCODING_TEMPLATE_SOURCE
			#define IR_ENCODING_TEMPLATE_SOURCE
			#include "../../source/template/encoding.h"
		#endif
		#ifndef IR_ENCODING_SOURCE
			#define IR_ENCODING_SOURCE
			#include "../../source/encoding.h"
		#endif
	#endif
#endif

///@}
#endif	//#ifndef IR_ENCODING