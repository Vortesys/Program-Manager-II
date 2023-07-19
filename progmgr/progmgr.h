/* * * * * * * *\
	PROGMGR.H -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's primary header file.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/* Includes */
#include <wtypes.h>

/* Macros and Defines */
#define CharSizeOf(x) (sizeof(x) / sizeof(*x))

/* Global Variables */
// PROGMGR.C
extern BOOL			bIsDefaultShell;
extern HINSTANCE	hAppInstance;
extern HWND			hwndProgMgr;
extern HWND			hwndMDIClient;
extern WCHAR		szAppTitle[32];
extern WCHAR		szProgMgr[];

/* Function Prototypes */
// WNDPROC.C
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
