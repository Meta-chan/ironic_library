/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_CODEC
#define IR_CODEC

#include <ir_container/ir_quiet_vector.h>

namespace ir
{
///@defgroup ir_codec Class responsible for all Ironic encoding functions. Use codecs 
///@{
	class Codec
	{
	private:
		static QuietVector<char> _buffer;
	
	public:
		///ASCII codec. See this codec for getting documentation for all other codecs
		class ASCII
		{
		public:
			///Encoding's native character
			typedef char Char;
			///Signed encoding's native character
			typedef signed char SignedChar;
			///Unsigned encoding's native character
			typedef unsigned char UnsignedChar;

			///Converts the Unicode code to the encoding
			///@param		code		Unicode code
			///@param		string		String in ASCII encoding. May be `nullptr`, then nothing is written
			///@param		nnative		Pointer to `size_t` that receives number of ASCII characters were written. May be `nullptr`
			///@return					`true` if `code` is representable in ASCII encoding
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;

			///Converts specified encoding to one Unicode code
			///@param[in]	string		String in ASCII encoding
			///@param[out]	nnative		Pointer to `size_t` that receives number of ASCII characters were read. May be `nullptr`, then nothing is written
			///@return					Unicode code
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF8 codec
		class UTF8
		{
		public:
			typedef char Char;
			typedef signed char SignedChar;
			typedef unsigned char UnsignedChar;
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF16 codec
		class UTF16
		{
		public:
			#ifdef _WIN32
				typedef wchar_t Char;
				typedef signed short int SignedChar;
				typedef wchar_t UnsignedChar;
			#else
				typedef unsigned short int Char;
				typedef signed short int SignedChar;
				typedef unsigned short int UnsignedChar;
			#endif
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///UTF32 codec
		class UTF32
		{
		public:
			typedef unsigned int Char;
			typedef signed int SignedChar;
			typedef unsigned int UnsignedChar;
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
		};
		
		///Windows-1251 (cyrillic) codec
		class CP1251
		{
		public:
			typedef char Char;
			typedef signed char SignedChar;
			typedef unsigned char UnsignedChar;
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
			
		private:
			static const unsigned short int _decode_table[0x40];
		};
		
		///DOS-866 (cyrillic) codec
		class CP866
		{			
		public:
			typedef char Char;
			typedef signed char SignedChar;
			typedef unsigned char UnsignedChar;
			static bool encode(unsigned int code, Char *string, size_t *nnative) noexcept;
			static unsigned int decode(const Char *string, size_t *nnative) noexcept;
			
		private:
			static const unsigned short int _decode_table1[0x30];
			static const unsigned short int _decode_table2[0x10];
		};
		
		///Decodes null-terminated source string into null-terminated destination string (null-character is written)
		///Replaces the symbols not representable in destination encoding with @c errcode
		///@tparam	SourceCodec	Source codec
		///@tparam	DestCodec	Destination codec
		///@param	source	Source string with encoding specified with @c codec1
		///@param	errcode	Default Unicode code, which is used when some symbol is not representable in destination encoding
		///@param	string2	Destination string. May be `nullptr`, then nothing is written
		///@return				Number of native characters of destination string (excluding null character)
		template<class SourceCodec, class DestCodec>
		static size_t recode(const typename SourceCodec::Char *source, unsigned int errcode, typename DestCodec::Char *dest) noexcept;

		///Same as `recode`, but returns a string, allocated with standard C `malloc`
		template<class SourceCodec, class DestCodec>
		static typename DestCodec::Char *alloc_recode(const typename SourceCodec::Char *source, unsigned int errcode) noexcept;

		///Same as `recode`, but returns a pointer to internal buffer. Be careful, there is only one internal buffer!
		template<class SourceCodec, class DestCodec>
		static typename DestCodec::Char *buffer_recode(const typename SourceCodec::Char *source, unsigned int errcode) noexcept;
		
		///Clear internal buffer
		static void clear() noexcept;
	};
///@}
}

#if (defined(IR_IMPLEMENT) || defined(IR_CODEC_IMPLEMENT)) && !defined(IR_CODEC_NOT_IMPLEMENT)
	#include <implementation/ir_codec_implementation.h>
#endif

///@}
#endif	//#ifndef IR_CODEC