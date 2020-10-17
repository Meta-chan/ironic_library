#include <Windows.h>

int main(int argc, char **argv)
{
	WORD attributes = 0;
	if (argc >= 2)
	{
		const char *s = argv[1];
		while (1)
		{
			if (*s == '\0') break;
			if (*s == 'r' || *s == 'R') attributes |= FOREGROUND_RED;
			if (*s == 'g' || *s == 'G') attributes |= FOREGROUND_GREEN;
			if (*s == 'b' || *s == 'B') attributes |= FOREGROUND_BLUE;
			s++;
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes);
	return 	0;
};