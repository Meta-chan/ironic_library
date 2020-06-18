/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranty
	To get help with installation, visit README
	Created by @meta-chan, k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

//ir_plot - plotting library with only one function.
//The function returns after you close the plot window

#ifndef IR_PLOT
#define IR_PLOT

#include <ir_errorcode.h>

namespace ir
{
///@defgroup ir_plot Plotting function
///@{

	///Callback function used by ir::plot
	///@param user Pointer to data given by user
	///@param index Index of point, <tt> 0 <= index < n </tt>
	///@return @c X or @c Y coordinate of the point 
	typedef double(GetValueFunction)(void *user, unsigned int index);

	///Input for @c ir::plot, represents set of points of graphic
	struct Plot
	{
		GetValueFunction *xfunc	= nullptr;	///< Function returning @c X coordinate
		GetValueFunction *yfunc	= nullptr;	///< Function returning @c Y coordinate
		void *xuser				= nullptr;	///< Data given to @c xfunc
		void *yuser				= nullptr;	///< Data given to @c yfunc			
		unsigned int n			= 0;		///< Number of points in graphic
		unsigned int color		= 0;		///< RGB color of line
	};

	///Plots graphs
	///@param nplots	Number of @c ir::Plot structures given to the function
	///@param plot1		First @c ir::Plot to draw
	ec plot(unsigned int nplots, Plot plot1, ...);
	
///@}
};

#if (defined(IR_IMPLEMENT) || defined (IR_PLOT_IMPLEMENT)) && !defined(IR_PLOT_NOT_IMPLEMENT)
	#include <implementation/ir_plot_implementation.h>
#endif

#endif	//#ifndef IR_PLOT