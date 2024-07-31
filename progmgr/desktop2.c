/* * * * * * * *\
	DESKTOP2.C -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Program Manager's desktop window.
		Displays running programs.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include <Windows.h>
#include "progmgr.h"

/* Structures */
typedef struct _DPI
{
	HWND hWnd;
	HICON hIcon;
	DWORD dwFlags;
} DPI, * PDPI;

/* * * *\
	CreateDesktopWindow2 -
		Creates the desktop window.
	RETURNS -
		TRUE if created successfully,
		FALSE if otherwise.
\* * * */
BOOL CreateDesktopWindow2(VOID)
{
	// TODO:
	// Create window, resize it to the size of the screen
	// possibly create child windows for other monitors
	// color the window invisible, get list of running
	// programs and then draw them all and capture their
	// system menus as context menus for each icon.

	// FUNCTIONS TODO:
	// QueryRunningPrograms()
	// DrawProgramIcon(DWORD dwFlags)
	// 
	return TRUE;
}