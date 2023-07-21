/* * * * * * * *\
	PROGMGR.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's main file, now with extra wWinMain!
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "resource.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <Windows.h>
#include <strsafe.h>
#include <Lmcons.h>
#include <security.h>

/* Variables */
// Global
BOOL		bIsDefaultShell = FALSE;
// Global Strings
WCHAR		szAppTitle[32];
WCHAR		szProgMgr[] = L"progmgr";
WCHAR		szWebsite[64];
// Window Related
HICON		hProgMgrIcon = NULL;
HINSTANCE	hAppInstance;
HWND		hWndProgMgr = NULL;
HWND		hWndMDIClient = NULL;

/* Functions */
/* * * *\
	wWinMain -
		Program Manager's entry point.
\* * * */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HANDLE hAccel;
	HMENU hMenu;
	HMENU hSystemMenu;
	WNDCLASS wc = { 0 };
	WCHAR szBuffer[MAX_PATH];
	WCHAR szClass[16];
	WCHAR szUsername[UNLEN + 1] = L"";
	DWORD dwUsernameLen = UNLEN;
	WCHAR szWindowTitle[UNLEN + ARRAYSIZE(szAppTitle) + 4] = L"";
	RECT rWorkArea;

	hAppInstance = hInstance;

	// Create Important Strings
	LoadString(hAppInstance, IDS_APPTITLE, szAppTitle, ARRAYSIZE(szAppTitle));
	LoadString(hAppInstance, IDS_PMCLASS, szClass, ARRAYSIZE(szClass));
	LoadString(hAppInstance, IDS_WEBSITE, szWebsite, ARRAYSIZE(szWebsite));

	// And add Task Manager...
	// 

	// Register the Frame Window
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hAppInstance;
	wc.hIcon = hProgMgrIcon = LoadImage(hAppInstance, MAKEINTRESOURCE(IDI_PROGMGR), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
	wc.lpszClassName = szClass;
	if (!RegisterClass(&wc))
		return FALSE;

	// Load the Accelerator table
	hAccel = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDA_ACCELS));
	if (!hAccel)
		return FALSE;

	// Perform Registry actions, close if registry is inaccessible.
	if (!InitializeRegistryKeys())
		return FALSE;

	bIsDefaultShell = IsProgMgrDefaultShell();
	
	GetUserNameEx(NameSamCompatible, szUsername, &dwUsernameLen);
	// OutputDebugString(szUsername);

	StringCchCopy(szWindowTitle, ARRAYSIZE(szAppTitle), szAppTitle);
	StringCchCat(szWindowTitle, ARRAYSIZE(szWindowTitle), L" - ");
	StringCchCat(szWindowTitle, ARRAYSIZE(szWindowTitle), szUsername);

	if (!CreateWindow(wc.lpszClassName,
		szWindowTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		160, 80, 640, 480, 0, 0, hAppInstance, NULL))
		return 2;

	// Load the menus...
	hMenu = GetMenu(hWndProgMgr);
	hSystemMenu = GetSystemMenu(hWndProgMgr, FALSE);

	if (bIsDefaultShell) {
		// Modify the context menus since we're the default shell
		DeleteMenu(hSystemMenu, SC_CLOSE, MF_BYCOMMAND);

		LoadString(hAppInstance, IDS_SHUTDOWN, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_SHUTDOWN, szBuffer);
		ModifyMenu(hMenu, IDM_FILE_EXIT, MF_BYCOMMAND | MF_STRING, IDM_SHUTDOWN, szBuffer);

		LoadString(hAppInstance, IDS_RUN, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_FILE_RUN, szBuffer);

		LoadString(hAppInstance, IDS_TASKMGR, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_TASKMGR, szBuffer);
	
		// Refresh the wallpaper...
		SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, szBuffer, 0);
		SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, szBuffer, SPIF_SENDCHANGE);

		// Reset the work area (removes dead-space after switching from another shell)
		// May have unintended consequences.
		SystemParametersInfo(SPI_SETWORKAREA, 0, (PVOID)&rWorkArea, SPIF_SENDCHANGE);
	}

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if (!TranslateMDISysAccel(hWndMDIClient, &msg) &&
			!TranslateAccelerator(hWndProgMgr, hAccel, &msg))
		{
			DispatchMessage(&msg);
		}
	}

	return 0;
}
