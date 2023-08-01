/* * * * * * * *\
	WNDPROC.C -
		Copyright (c) 2023 freedom7341, Vortesys
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
	switch (message)
	{

	case WM_CREATE:
	{
		RECT rc;
		CLIENTCREATESTRUCT ccs;

		// Frame Window
		hWndProgMgr = hWnd;

		// MDI Client Window
		ccs.hWindowMenu = GetSubMenu(GetMenu(hWnd), IDM_WINDOW);
		ccs.idFirstChild = IDM_WINDOW_CHILDSTART;

		GetClientRect(hWndProgMgr, &rc);

		hWndMDIClient = CreateWindow(L"MDIClient", NULL,
			WS_CLIPCHILDREN | WS_CHILD | WS_VSCROLL | WS_HSCROLL,
			// rc.left - 1, rc.top - 1, rc.right + 2, rc.bottom + 2,
			0, 0, 0, 0,
			hWnd, (HMENU)1, hAppInstance, (LPWSTR)&ccs);

		if (!hWndMDIClient) {
			return -1;
		}
		break;
	}

	case WM_SYSCOMMAND:
	{
		if (wParam == IDM_SHUTDOWN)
		{
			CmdProc(hWnd, wParam, lParam);
			break;
		}
		if (wParam == IDM_TASKMGR)
		{
			ShellExecute(hWndProgMgr, L"open", L"TASKMGR.EXE", NULL, NULL, SW_NORMAL);
			break;
		}
		if (wParam == IDM_FILE_EXIT)
		if (wParam == IDM_FILE_RUN)
		{
			CmdProc(hWnd, wParam, lParam);
			break;
		}
		goto WndProcDefault;
		//return DefFrameProc(hWnd, hwndMDIClient, uiMsg, wParam, lParam);
	}

	case WM_COMMAND:
		if (CmdProc(hWnd, wParam, lParam))
		{
			break;
		}
		goto WndProcDefault;

	case WM_CLOSE:
		if (GetShellWindow())
			SetShellWindow(0);
		PostQuitMessage(0);
		break;

	case WM_ENDSESSION:
		PostQuitMessage(0);
		break;

	default:
WndProcDefault:
		return DefFrameProc(hWnd, hWndMDIClient, message, wParam, lParam);
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
	switch (GET_WM_COMMAND_ID(wParam, lParam))
	{

	case IDM_SHUTDOWN:
		ExitWindowsDialog(hWnd);
		break;
		
	case IDM_FILE_RUN:
		RunFileDlg(hWnd, NULL, NULL, NULL, NULL, RFF_CALCDIRECTORY);
		break;

	case IDM_FILE_EXIT:
		PostQuitMessage(0);
		break;

	case IDM_OPTIONS_AUTOARRANGE:
		bAutoArrange = !bAutoArrange;
		UpdateChecks(bAutoArrange, IDM_OPTIONS, IDM_OPTIONS_AUTOARRANGE);
		goto SaveConfig;

	case IDM_OPTIONS_MINONRUN:
		bMinOnRun = !bMinOnRun;
		UpdateChecks(bMinOnRun, IDM_OPTIONS, IDM_OPTIONS_MINONRUN);
		goto SaveConfig;

	case IDM_OPTIONS_TOPMOST:
		bTopMost = !bTopMost;
		SetWindowPos(hWndProgMgr, bTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE);
		UpdateChecks(bTopMost, IDM_OPTIONS, IDM_OPTIONS_TOPMOST);
		goto SaveConfig;

	case IDM_OPTIONS_SHOWUSERNAME:
		bShowUsername = !bShowUsername;
		UpdateChecks(bShowUsername, IDM_OPTIONS, IDM_OPTIONS_SHOWUSERNAME);
		goto SaveConfig;

	case IDM_OPTIONS_SAVESETTINGS:
		bSaveSettings = !bSaveSettings;
		UpdateChecks(bSaveSettings, IDM_OPTIONS, IDM_OPTIONS_SAVESETTINGS);
		goto SaveConfig;

	case IDM_OPTIONS_SAVENOW:
SaveConfig:
		SaveConfig();
		break;

	case IDM_HELP_INDEX:
		ShellExecute(NULL, L"open", szWebsite, NULL, NULL, SW_SHOWNORMAL);
		break;

	case IDM_HELP_ABOUT:
	{
		WCHAR szTitle[40];

		LoadString(hAppInstance, IDS_APPTITLE, szTitle, ARRAYSIZE(szTitle));
		ShellAbout(hWndProgMgr, szTitle, NULL, hProgMgrIcon);
		break;
	}

	default:
		return FALSE;
	}

	return TRUE;
}

/* * * *\
	UpdateChecks -
		Updated checkmarks for a menu based on input
	RETURNS -
		Nothing!
\* * * */
VOID UpdateChecks(BOOL bVarMenu, UINT uSubMenu, UINT uID)
{
	HMENU hMenu;

	hMenu = GetMenu(hWndProgMgr);
	CheckMenuItem(hMenu, uID, (WORD)(bVarMenu ? MF_CHECKED : MF_UNCHECKED));

	return;
}

/* * * *\
	ChildWndProc -
		Program Manager's window procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}