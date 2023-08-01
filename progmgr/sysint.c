/* * * * * * * *\
	SYSINT.C -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program Manager's system interface functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Functions */

/* * * *\
	RunFileDlg -
		Produces a Run dialog window.
	RETURNS -
		True if successful, false if unsuccessful.
\* * * */
BOOL RunFileDlg(HWND hWndOwner,
	HICON	hIcon,
	LPWSTR	lpszDir,
	LPWSTR	lpszTitle,
	LPWSTR	lpszDesc,
	DWORD	dwFlags)
{
	HMODULE hLib = LoadLibrary(L"shell32.dll");

	if (hLib)
	{
		FARPROC fLib = GetProcAddress(hLib, MAKEINTRESOURCEA(61));
		if (fLib(hWndOwner, hIcon, lpszDir, lpszTitle, lpszDesc, dwFlags))
		{
			FreeLibrary(hLib);
			return TRUE;
		}
	}

	return FALSE;
}

/* * * *\
	ExitWindowsDialog  -
		Produces a Shutdown dialog window.
	RETURNS -
		True if successful, false if unsuccessful.
\* * * */
BOOL ExitWindowsDialog(HWND hWndOwner)
{
	HMODULE hLib = LoadLibrary(L"shell32.dll");

	if (hLib)
	{
		FARPROC fLib = GetProcAddress(hLib, (LPCSTR)60);
		if (fLib(hWndOwner))
		{
			FreeLibrary(hLib);
			return TRUE;
		}
	}

	return FALSE;
}

/* * * *\
	SetShellWindow -
		Sets a window as the shell window.
	RETURNS -
		True if successful, false if unsuccessful.
\* * * */
BOOL SetShellWindow(HWND hWndShell)
{
	HMODULE hLib = GetModuleHandle(L"user32.dll");

	if (hLib)
	{
		FARPROC fLib = GetProcAddress(hLib, "SetShellWindow");
		if (fLib(hWndShell)) 
		{
			return TRUE;
		}
	}

	return FALSE;
}
