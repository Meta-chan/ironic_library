#define IR_INCLUDE 'a'
#include "../include/ir/encoding.h"
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

void printutf32(const ir::char32 *s)
{
	while (*s != 0)
	{
		printf("%u ", *s);
		s++;
	}
	printf("\n");
}

int main(void)
{
	const char *hello = "Hello Привет こんにちは!";
	
	printf("Original: %s\n", hello);
	printf("ASCII: %s\n", ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::ASCII>(hello, "*"));
	printf("UTF8: %s\n", ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::UTF8>(hello, "*"));
	printf("UTF16: %ls\n", ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::UTF16>(hello, L"*"));
	printf("UTF32: "); printutf32(ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::UTF32>(hello));
	printf("1251: %s\n", ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::CP1251>(hello, "*"));
	printf("866: %s\n", ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::CP866>(hello, "*"));

	#ifdef _WIN32
		const wchar_t *whello = ir::Encoding::buffer_recode<ir::Encoding::UTF8, ir::Encoding::UTF16>(hello, L"*");
		DWORD written;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), whello, (DWORD)wcslen(whello), &written, (LPVOID)0);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), whello, (DWORD)wcslen(whello), &written, (LPVOID)0);
	#endif

	ir::Encoding::clear();
	getchar();

	return 0;
}