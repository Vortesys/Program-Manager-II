/* * * * * * * *\
	PROGMGR.H -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Program Manager's primary header file.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "Version.lib")

/* Includes */
#include <wtypes.h>

/* Macros and Defines */
#define GET_WM_COMMAND_ID(wParam, lParam) LOWORD(wParam)
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
extern BOOL			bIsDefaultShell;
// Handles
extern HINSTANCE	hAppInstance;
extern HANDLE		hAppHeap;
extern HWND			hWndProgMgr;
// Icons
extern HICON		hProgMgrIcon;
extern HICON		hGroupIcon;
// Strings
extern WCHAR		szAppTitle[64];
extern WCHAR		szProgMgr[];
extern WCHAR		szWebsite[64];
extern WCHAR		szClass[16];
// Permissions
extern BOOL bPermAdmin; // Has Administrator permissions
extern BOOL bPermGuest; // Has Guest permissions
extern BOOL bPermPower; // Has power option permissions

/* Function Prototypes */
BOOL UpdatePermissions(VOID);
// DESKTOP.C
BOOL CreateDesktopWindow(VOID);
// SYSINT.C
BOOL RunFileDlg(HWND hWndOwner, HICON hIcon, LPWSTR lpszDir, LPWSTR lpszTitle, LPWSTR lpszDesc, DWORD dwFlags);
BOOL ExitWindowsDialog(HWND hWndOwner);
BOOL SetShellWindow(HWND hWndShell);
// WNDPROC.C
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CmdProc(HWND hWnd, WPARAM wParam, LPARAM lParam);
VOID UpdateChecks(BOOL bVarMenu, UINT uSubMenu, UINT uID);
VOID UpdateWindowTitle(VOID);
