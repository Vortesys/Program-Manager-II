/* * * * * * * *\
	DIALOG.C -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program Manager's dialogs and related
		common functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "dialog.h"
#include "resource.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Functions */


/* * * *\
	GroupDlgProc -
		Dialog procedure for creating or modifying
		a group.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK GroupDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_INITDIALOG:

		// Place message cases here. 

	default:
		return FALSE;
	}
}

/* * * *\
	ItemDlgProc -
		Dialog procedure for creating or modifying
		an item.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK ItemDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_INITDIALOG:

		// Place message cases here. 

	default:
		return FALSE;
	}
}
