/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranity
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir_plot - light plotting library with only one function

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
	#include "ir_plot_implementation.h"
#endif

#endif	//#ifndef IR_PLOT