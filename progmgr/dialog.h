/* * * * * * * *\
	DIALOG.H -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Dialog function prototypes and related
		common dialog functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Function Prototypes */
BOOL CALLBACK NewGroupDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK NewItemDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ShutdownDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam);
