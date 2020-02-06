/*
	Part of the Ironic Project. Redistributed under MIT License, which means:
		- Do whatever you want
		- Please keep this notice and include the license file to your project
		- I provide no warranity
	To get help with installation, visit README
	Created by Meta-chan, @k.sovailo@gmail.com
	Reinventing bicycles since 2020
*/

#ifndef IR_PLOT_IMPLEMENATTION
#define IR_PLOT_IMPLEMENATTION

#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>

struct
{
	ir::Plot *plots;
	unsigned int nplots;
	double minx, miny, maxx, maxy;
} currentplots;

void plotwindowpaint(HWND hWnd)
{
	//Begin paint
	PAINTSTRUCT paintstruct;
	HDC hdc = BeginPaint(hWnd, &paintstruct);
	RECT windowrect;
	GetClientRect(hWnd, &windowrect);

	for (unsigned int nplot = 0; nplot < currentplots.nplots; nplot++)
	{
		ir::Plot *p = currentplots.plots + nplot;
		if (p->n > 1)
		{
			//Move to first point
			double x = p->xfunc(p->xuser, 0);
			double y = p->yfunc(p->yuser, 0);
			MoveToEx(hdc, (int)(windowrect.right * (x - currentplots.minx) / (currentplots.maxx - currentplots.minx)),
				(int)(windowrect.bottom - windowrect.bottom * (y - currentplots.miny) / (currentplots.maxy - currentplots.miny)), nullptr);

			//Choose pen
			HPEN hpen = CreatePen(PS_SOLID, 0, p->color);
			HPEN holdpen = (HPEN)SelectObject(hdc, hpen);
			DeleteObject(holdpen);

			//Draw other points
			for (unsigned int i = 1; i < p->n; i++)
			{
				x = p->xfunc(p->xuser, i);
				y = p->yfunc(p->yuser, i);
				LineTo(hdc, (int)(windowrect.right * (x - currentplots.minx) / (currentplots.maxx - currentplots.minx)),
					(int)(windowrect.bottom - windowrect.bottom * (y - currentplots.miny) / (currentplots.maxy - currentplots.miny)));
			}
		}
	}

	EndPaint(hWnd, &paintstruct);
};

LRESULT CALLBACK plotwindowproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		plotwindowpaint(hWnd);
		break;
	case WM_LBUTTONUP:
		MessageBox(hWnd, TEXT("Вы кликнули!"), TEXT("событие"), 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
};

ir::ec ir::plot(unsigned int nplots, Plot plot1, ...)
{
	if (nplots == 0) return ec::ec_ok;

	//Copying parameters to global
	currentplots.nplots = nplots;
	currentplots.plots = (Plot*)malloc(nplots * sizeof(Plot));
	if (currentplots.plots == nullptr) return ec::ec_alloc;
	memcpy(currentplots.plots, &plot1, nplots * sizeof(Plot));

	//Searching minimum and maximum
	currentplots.minx = INFINITY;
	currentplots.miny = INFINITY;
	currentplots.maxx = -INFINITY;
	currentplots.maxy = -INFINITY;
	for (unsigned int nplot = 0; nplot < nplots; nplot++)
	{
		ir::Plot *p = &plot1 + nplot;
		for (unsigned int i = 0; i < p->n; i++)
		{
			double x = p->xfunc(p->xuser, i);
			if (x < currentplots.minx) currentplots.minx = x;
			if (x > currentplots.maxx) currentplots.maxx = x;

			double y = p->yfunc(p->yuser, i);
			if (y < currentplots.miny) currentplots.miny = y;
			if (y > currentplots.maxy) currentplots.maxy = y;
		}
	}

	//Modifying maximum and minimum
	double interval = currentplots.maxx - currentplots.minx;
	if (interval == 0) interval = 1.0;
	else interval *= 0.1;
	currentplots.minx -= interval;
	currentplots.maxx += interval;

	interval = currentplots.maxy - currentplots.miny;
	if (interval == 0) interval = 1.0;
	else interval *= 0.1;
	currentplots.miny -= interval;
	currentplots.maxy += interval;

	//Registering class
	LPCTSTR classname = TEXT("ir_plot_window_class");
	WNDCLASS windowclass = { 0 };
	windowclass.lpfnWndProc = plotwindowproc;
	windowclass.style = CS_HREDRAW | CS_VREDRAW;
	windowclass.hInstance = GetModuleHandle(NULL);
	windowclass.lpszClassName = classname;
	windowclass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	if (RegisterClass(&windowclass) == 0) return ec::ec_windows_register_class;

	//Creating window
	RECT screen;
	GetWindowRect(GetDesktopWindow(), &screen);
	HWND hWnd = CreateWindow(classname, TEXT("ir_plot"), WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		screen.right / 4, screen.bottom / 4, screen.right / 2, screen.bottom / 2,
		NULL, NULL, GetModuleHandle(NULL), NULL);
	if (!hWnd) return ec::ec_windows_create_window;

	//Displaying window
	MSG msg = { 0 };
	int ok = 0;
	while ((ok = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (ok == -1) return ec::ec_windows_getmessage;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return ec::ec_ok;
};

#endif //#ifndef IR_PLOT_IMPLEMENATTION