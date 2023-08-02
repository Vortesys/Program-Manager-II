/* * * * * * * *\
	GROUP.H -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program group and item structures and... stuff.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Includes */
#include <wtypes.h>

/* Function Prototypes */
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND TempCreateGroup(HWND hMDIClient);