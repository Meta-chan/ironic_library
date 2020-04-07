#define IR_IMPLEMENT
#include "ir_utf.h"
#include <stdio.h>
#ifdef _WIN32
	#include <Windows.h>
#endif

//The test in absolutely not convenient, but the library works.
//You have to believe me.
//Everyting here depend on compiler, the encoding in which the file was saved, the phase of the moon etc.
//But it works
int main(void)
{
	utf_init();
	utf_1251.init();
	utf_c.init();
	utf_utf8.init();
	utf_utf16.init();
	utf_utf32.init();
	
	const char *hello = "Hello World! Привет, мир! こんにちは世界!\n";
	const wchar_t *hellow = L"Hello World! Привет, мир! こんにちは世界!\n";

	printf("Original: %s", hello);
	wprintf(L"Original W: %ls", hellow);
	printf("utf_c: %s", utf_alloc_recode(&utf_utf8, hello, '*', &utf_c));
	printf("utf_1251: %s", utf_alloc_recode(&utf_utf8, hello, '*', &utf_1251));
	printf("utf_utf8: %s", utf_alloc_recode(&utf_utf8, hello, '*', &utf_utf8));
	wprintf(L"utf_utf16: %ls", utf_alloc_recode(&utf_utf8, hello, '*', &utf_utf16));
	
	#ifdef _WIN32
		printf("WriteConsole, original w: ");
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), hellow, wcslen(hellow), NULL, NULL);
		printf("WriteConsole, utf_utf16: ");
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), utf_alloc_recode(&utf_utf8, hello, '*', &utf_utf16),
			wcslen(hellow), NULL, NULL);
	#endif
	
		unsigned int *u32 = (unsigned int*)utf_alloc_recode(&utf_utf8, hello, '*', &utf_utf32);
	while (*u32 != 0) {	printf("%u ", *u32); u32++; }
	printf("\n");

	return 0;
};
