/* * * * * * * *\
	WNDPROC.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's window procedure.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "resource.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Functions */

/* * * *\
	WndProc -
		Program Manager's window procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) {

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

	case WM_SYSCOMMAND:
	{
		// if (wParam == IDM_TASKMGR) {
		//ShellExecute(hwndProgMgr, &TEXT("OPEN\0"), &TEXT("TASKMGR.EXE\0"), NULL, NULL, SW_NORMAL);
			// break;
		// }
		//break;
		if (wParam == IDM_FILE_SHUTDOWN) {
			CmdProc(hWnd, wParam, lParam);
			break;
		}
		if (wParam == IDM_FILE_RUN) {
			CmdProc(hWnd, wParam, lParam);
			break;
		}
		goto WndProcDefault;
		//return DefFrameProc(hWnd, hwndMDIClient, uiMsg, wParam, lParam);
	}

	case WM_COMMAND:
		if (CmdProc(hWnd, wParam, lParam)) {
			break;
		}
		goto WndProcDefault;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
WndProcDefault:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

/* * * *\
	CmdProc -
		Program Manager's syscommand procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK CmdProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (GET_WM_COMMAND_ID(wParam, lParam)) {

	case IDM_FILE_RUN:
		RunFile(hWnd, NULL, NULL, NULL, NULL, RFF_CALCDIRECTORY);
		break;

	case IDM_FILE_SHUTDOWN:
		ShutdownDlg(hWnd);
		break;

	case IDM_HELP_INDEX:
		ShellExecute(NULL, L"open", szWebsite, NULL, NULL, SW_SHOWNORMAL);
		break;

	case IDM_HELP_ABOUT:
	{
		WCHAR szTitle[40];
		OutputDebugString(L"ABOUT BALLS!");
		HICON hIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(PROGMGRICON));

		LoadString(hAppInstance, IDS_APPTITLE, szTitle, CharSizeOf(szTitle));
		ShellAbout(hwndProgMgr, szTitle, NULL, hIcon);
		break;
	}

	default:
		return FALSE;
	}

	return TRUE;
}