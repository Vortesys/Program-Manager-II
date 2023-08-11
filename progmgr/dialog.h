/* * * * * * * *\
	DIALOG.H -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Dialog function prototypes and related
		common dialog functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Function Prototypes */
BOOL CALLBACK ItemDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GroupDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
