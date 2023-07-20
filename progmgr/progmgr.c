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
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	HANDLE hAccel;
	WNDCLASS wc = { 0 };
	WCHAR szClass[16];
	WCHAR szUsername[UNLEN + 1] = L"";
	DWORD dwUsernameLen = UNLEN;
	WCHAR szWindowTitle[UNLEN + ARRAYSIZE(szAppTitle) + 4] = L"";

	hAppInstance = hInstance;

	// Create Important Strings
	LoadString(hAppInstance, IDS_APPTITLE, szAppTitle, CharSizeOf(szAppTitle));
	LoadString(hAppInstance, IDS_PMCLASS, szClass, CharSizeOf(szClass));
	LoadString(hAppInstance, IDS_WEBSITE, szWebsite, CharSizeOf(szWebsite));

	// And add Task Manager...
	// LoadString(hAppInstance, IDS_TASKMGR, szBuffer, CharSizeOf(szBuffer));
	// InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_TASKMGR, szBuffer);

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

	InitializeRegistryKeys();
	//if (IsProgMgrDefaultShell) {

	//}

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

	//hSystemMenu = GetSystemMenu(hwndProgman, FALSE);

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
