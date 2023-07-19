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
#include <Windows.h>

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
HWND		hwndProgMgr = NULL;
HWND		hwndMDIClient = NULL;

/* Program Entry Point */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	WCHAR szClass[16];

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
	wc.hIcon = hProgMgrIcon = LoadImage(hAppInstance, MAKEINTRESOURCE(PROGMGRICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
	wc.lpszClassName = szClass;
	if (!RegisterClass(&wc))
		return 1;

	InitializeRegistryKeys();
	//if (IsProgMgrDefaultShell) {

	//}

	if (!CreateWindow(wc.lpszClassName,
		szAppTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		160, 80, 640, 480, 0, 0, hInstance, NULL))
		return 2;

	//hSystemMenu = GetSystemMenu(hwndProgman, FALSE);

	while (GetMessage(&msg, NULL, 0, 0) > 0)
		DispatchMessage(&msg);

	return 0;
}
