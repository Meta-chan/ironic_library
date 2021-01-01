#define IR_IMPLEMENT
#include <ir_codec.h>
#ifdef _WIN32
	#include <Windows.h>
#endif
#include <stdio.h>

//So, algorithm of Visual studio compiler:
//UTF8, no BOM -> strings in normal UTF8, wide strings are trash
//UTF8/UTF16LE/UTF16BE, with BOM -> VS tries to convert source in host encoding (1251 in my case)
//If convertion fails, '?' is used
//Wide strings are normal UTF16 Little Endian

//So, for test the source must be saved in UTF8 without BOM
//It is the most predictable and cross-platform option

void printutf32(const unsigned int *s)
{
	while (*s != 0)
	{
		printf("%u ", *s);
		s++;
	}
}

int main(void)
{
	const char *hello = "Hello Привет こんにちは!";
	
	printf("Original: %s\n", hello);
	printf("ASCII: %s\n", ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::ASCII>(hello, '*'));
	printf("UTF8: %s\n", ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::UTF8>(hello, '*'));
	printf("UTF16: %ls\n", ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::UTF16>(hello, '*'));
	printf("UTF32: "); printutf32(ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::UTF32>(hello, '*')); printf("\n");
	printf("1251: %s\n", ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::CP1251>(hello, '*'));
	printf("866: %s\n", ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::CP866>(hello, '*'));

	#ifdef _WIN32
		const wchar_t *whello = ir::Codec::buffer_recode<ir::Codec::UTF8, ir::Codec::UTF16>(hello, '*');
		DWORD written;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), whello, wcslen(whello), &written, (LPVOID)0);
	#endif

	ir::Codec::clear();
	getchar();

	return 0;
}
