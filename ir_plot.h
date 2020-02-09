/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir_plot - plotting library with only one function.
//The function returns after you close the plot window

#ifndef IR_PLOT
#define IR_PLOT

#include "ir_errorcode.h"

namespace ir
{
	typedef double(GetValueFunction)(void *user, unsigned int i);

	struct Plot
	{
		GetValueFunction *xfunc = nullptr;
		GetValueFunction *yfunc = nullptr;
		void *xuser = nullptr;
		void *yuser = nullptr;
		unsigned int n = 0;
		unsigned int color = 0;
	};

	ec plot(unsigned int nplots, Plot plot1, ...);
};

#if (defined(IR_IMPLEMENT) || defined (IR_PLOT_IMPLEMENT))
	#include "implementation/ir_plot_implementation.h"
#endif

#endif	//#ifndef IR_PLOT