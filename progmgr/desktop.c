/* * * * * * * *\
	DESKTOP.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's desktop window.
		Only visible as the default shell.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "resource.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Variables */
HWND hWndDesktop;
RECT rcRoot;

/* Functions */

/* * * *\
	CreateDesktopWindow -
		Creates the invisible desktop window.
	RETURNS -
		True if successful, false if unsuccessful.
\* * * */
BOOL CreateDesktopWindow()
{
	MSG msg = { 0 };
	WNDCLASSEX wc = { 0 };
	WCHAR szBuffer[MAX_PATH];
	WCHAR szClass[16];

	// If we fail, we shouldn't be here anyways.
	//if (GetShellWindow())
	//	return FALSE;

	// Get size of the root HWND
	GetWindowRect(GetDesktopWindow(), &rcRoot);

	// Load the window class string
	LoadString(hAppInstance, IDS_DTCLASS, szClass, ARRAYSIZE(szClass));

	// Refresh the wallpaper...
	SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, szBuffer, 0);
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, szBuffer, SPIF_SENDCHANGE);

	// Reset the work area (removes dead-space after switching from another shell)
	// May have unintended consequences. Doesn't work with multiple monitors.
	SystemParametersInfo(SPI_SETWORKAREA, 0, &rcRoot, SPIF_SENDCHANGE);

	// Register the desktop window
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = DeskWndProc;
	wc.hInstance = hAppInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(30);
	wc.style = CS_NOCLOSE;
	wc.lpszClassName = szClass;
	if (!RegisterClassEx(&wc))
		return FALSE;

	if (!CreateWindowEx(WS_EX_TOOLWINDOW, wc.lpszClassName, NULL, WS_VISIBLE,
		rcRoot.left, rcRoot.top, rcRoot.right - rcRoot.left, rcRoot.bottom - rcRoot.top,
		NULL, NULL, hAppInstance, NULL));
		return 2;

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		DispatchMessage(&msg);
	}

	return TRUE;
}

/* * * *\
	DeskWndProc -
		Program Manager's desktop window procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK DeskWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_CREATE:
		// Populate hWndDesktop
		hWndDesktop = hWnd;

		// Set the window position to shell
		SetShellWindow(hWndDesktop);
		ShowWindow(hWndDesktop, SW_MAXIMIZE);

		// Strip the border from the desktop window
		SetWindowLongPtr(hWndDesktop, GWL_STYLE, WS_POPUP);

		// Size the window to take up the primary monitor's desktop
		SetWindowPos(hWndDesktop, HWND_BOTTOM,
			rcRoot.left, rcRoot.top, rcRoot.right - rcRoot.left, rcRoot.bottom - rcRoot.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);

		// Finally, bring Program Manager to the foreground.
		SetWindowPos(hWndProgMgr, HWND_TOP,
			0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		break;

	case WM_ACTIVATE:
	case WM_DPICHANGED:
	case WM_DISPLAYCHANGE:
	case WM_DEVICECHANGE:
		// Get size of the root HWND
		GetWindowRect(GetDesktopWindow(), &rcRoot);
		SetWindowPos(hWndDesktop, HWND_BOTTOM,
			rcRoot.left, rcRoot.top, rcRoot.right - rcRoot.left, rcRoot.bottom - rcRoot.top,
			SWP_NOACTIVATE);
		break;

	case WM_WINDOWPOSCHANGING:
	case WM_SYSCOMMAND:
	case WM_COMMAND:
		break;

	case WM_DESTROY:
	case WM_CLOSE:
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}