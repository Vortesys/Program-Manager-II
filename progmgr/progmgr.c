/* * * * * * * *\
	PROGMGR.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's main file, including wWinMain!
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
// Miscellaneous
BOOL		bIsDefaultShell = FALSE;
// Window Related
HICON		hProgMgrIcon = NULL;
HINSTANCE	hAppInstance;
HWND		hwndProgMgr = NULL;
HWND		hwndMDIClient = NULL;
// Global Strings
WCHAR		szAppTitle[32];

/* Registry Keys */

// ...
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

	if (!CreateWindow(wc.lpszClassName,
		szAppTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 640, 480, 0, 0, hInstance, NULL))
		return 2;

	while (GetMessage(&msg, NULL, 0, 0) > 0)
		DispatchMessage(&msg);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
	{
		RECT rc;
		CLIENTCREATESTRUCT ccs;

		hwndProgMgr = hWnd;

		// ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), IDM_WINDOW);
		// ccs.idFirstChild = IDM_CHILDSTART;

		GetClientRect(hwndProgMgr, &rc);

		hwndMDIClient = CreateWindow(TEXT("MDIClient"),
			NULL,
			WS_CLIPCHILDREN | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER,
			rc.left - 1, rc.top - 1,
			rc.right + 2, rc.bottom + 2,
			hWnd, (HMENU)1, hAppInstance,
			(LPTSTR)&ccs);
		if (!hwndMDIClient) {
			return -1;
		}

		break;
	}
	//case WM_SYSCOMMAND:
	//{
		// if (wParam == IDM_TASKMGR) {
		//ShellExecute(hwndProgMgr, &TEXT("OPEN\0"), &TEXT("TASKMGR.EXE\0"), NULL, NULL, SW_NORMAL);
			// break;
		// }
		//break;
	//}
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
