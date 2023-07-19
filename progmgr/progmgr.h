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
#define GET_WM_COMMAND_ID(wParam, lParam) LOWORD(wParam)
#define CharSizeOf(x) (sizeof(x) / sizeof(*x))
// RunFileDlg flags
#define RFF_NOBROWSE		0x0001	// Removes the browse button
#define RFF_NODEFAULT		0x0002	// No default item selected
#define RFF_CALCDIRECTORY	0x0004	// Calculates the working directory from the file name
#define RFF_NOLABEL			0x0008	// Removes the edit box label
#define RFF_NOSEPARATEMEM	0x0020  // Removes the Separate Memory Space check box, NT only
// RunFileDlg notification return values
#define RF_OK		        0x0000	// Allow the application to run
#define RF_CANCEL	        0x0001	// Cancel the operation and close the dialog
#define RF_RETRY            0x0002	// Cancel the operation, but leave the dialog open

/* Global Variables */
// PROGMGR.C
extern BOOL			bIsDefaultShell;
extern HINSTANCE	hAppInstance;
extern HWND			hwndProgMgr;
extern HWND			hwndMDIClient;
extern WCHAR		szAppTitle[32];
extern WCHAR		szProgMgr[];
extern WCHAR		szWebsite[64];

/* Function Prototypes */
// SHELLINT.C
BOOL RunFile(HWND hWndOwner, HICON hIcon, LPWSTR lpszDir, LPWSTR lpszTitle, LPWSTR lpszDesc, DWORD dwFlags);
BOOL ShutdownDlg(HWND hWndOwner);
// WNDPROC.C
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CmdProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
