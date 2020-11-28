/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by github.com/Meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_RESERVE_IMPLEMENTATION
#define IR_RESERVE_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

unsigned char reserve(void **pdata, unsigned int *preserved, unsigned int toreserve)
{
	if (*preserved < toreserve)
	{
		void *data;
		memcpy(&data, pdata, sizeof(void*));
		if (data == NULL) data = malloc(toreserve);
		else data = realloc(data, toreserve);
		memcpy(pdata, &data, sizeof(void*));
		if (data == NULL)
		{
			*preserved = 0;
			return 0;
		}
		else *preserved = toreserve;
	}
	return 1;
}

#endif	//#ifndef IR_RESERVE_IMPLEMENTATION