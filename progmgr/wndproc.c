/* * * * * * * *\
	WNDPROC.C -
		Copyright (c) 2023 Vortesys
	DESCRIPTION -
		Program Manager's window procedure.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "dialog.h"
#include "group.h"
#include "resource.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <Windows.h>
#include <strsafe.h>
#include <Lmcons.h>
#include <security.h>

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
		return TRUE;
	}

	case WM_SYSCOMMAND:
	{
		if ((wParam >= IDM_MAIN) && (wParam <= IDM_TASKMGR))
		{
			if (wParam == IDM_TASKMGR)
			{
				ShellExecute(hWndProgMgr, TEXT("open"), TEXT("TASKMGR.EXE"), NULL, NULL, SW_NORMAL);
				return 0;
			}

			if (CmdProc(hWnd, wParam, lParam))
				return 0;
		}

		return DefFrameProc(hWnd, hWndMDIClient, message, wParam, lParam);
	}

	case WM_COMMAND:
		if (CmdProc(hWnd, wParam, lParam))
			return 0;

		return DefFrameProc(hWnd, hWndMDIClient, message, wParam, lParam);

	case WM_CLOSE:
		if (bSaveSettings)
			SaveConfig(TRUE, TRUE, TRUE, TRUE);

		if (bIsDefaultShell && (GetShellWindow() != NULL))
			SetShellWindow(0);

		PostQuitMessage(0);
		break;

	case WM_ENDSESSION:
		PostQuitMessage(0);
		break;

	default:
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
		DialogBox(hAppInstance, MAKEINTRESOURCE(DLG_POWER), hWnd, (DLGPROC)ShutdownDlgProc);
		break;

	case IDM_FILE_NEW_GROUP:
		DialogBox(hAppInstance, MAKEINTRESOURCE(DLG_GROUP), hWnd, (DLGPROC)NewGroupDlgProc);
		break;

	case IDM_FILE_NEW_ITEM:
		DialogBox(hAppInstance, MAKEINTRESOURCE(DLG_ITEM), hWnd, (DLGPROC)NewItemDlgProc);
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
		SetWindowPos(hWndProgMgr, bTopMost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		UpdateChecks(bTopMost, IDM_OPTIONS, IDM_OPTIONS_TOPMOST);
		goto SaveConfig;

	case IDM_OPTIONS_SHOWUSERNAME:
		bShowUsername = !bShowUsername;
		UpdateWindowTitle();
		UpdateChecks(bShowUsername, IDM_OPTIONS, IDM_OPTIONS_SHOWUSERNAME);
		goto SaveConfig;

	case IDM_OPTIONS_SAVESETTINGS:
		bSaveSettings = !bSaveSettings;
		UpdateChecks(bSaveSettings, IDM_OPTIONS, IDM_OPTIONS_SAVESETTINGS);
		goto SaveConfig;

	case IDM_OPTIONS_SAVENOW:
	SaveConfig:
		SaveConfig(TRUE, TRUE, TRUE, FALSE);
		break;

	case IDM_WINDOW_CASCADE:
		SendMessage(hWndMDIClient, WM_MDICASCADE, 0, 0);
		break;

	case IDM_WINDOW_TILE:
		SendMessage(hWndMDIClient, WM_MDITILE, MDITILE_VERTICAL, 0);
		break;

	case IDM_WINDOW_TILEHORIZONTALLY:
		SendMessage(hWndMDIClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
		break;

	case IDM_WINDOW_ARRANGEICONS:
		SendMessage(hWndMDIClient, WM_MDIICONARRANGE, 0, 0);
		break;

	case IDM_HELP_INDEX:
		ShellExecute(NULL, TEXT("open"), szWebsite, NULL, NULL, SW_SHOWNORMAL);
		break;

	case IDM_HELP_ABOUT:
	{
		WCHAR szTitle[40];

		LoadString(hAppInstance, IDS_APPTITLE, szTitle, ARRAYSIZE(szTitle));
		ShellAbout(hWndProgMgr, szTitle, NULL, hProgMgrIcon);
		break;
	}

	default:
		if (GET_WM_COMMAND_ID(wParam, lParam) >= IDM_WINDOW_CHILDSTART)
		{
			DefFrameProc(hWnd, hWndMDIClient, 0, wParam, lParam);
		}
		else
		{
			HWND hChild = (HWND)SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, 0);
			if (hChild)
			{
				SendMessage(hChild, WM_COMMAND, wParam, lParam);
			}
		}
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
	UpdateWindowTitle -
		Updates Window title based on settings... and
		current foreground group window if applicable?
	RETURNS -
		Nothing!
\* * * */
VOID UpdateWindowTitle(VOID)
{
	WCHAR szUsername[UNLEN + 1] = TEXT("");
	DWORD dwUsernameLen = UNLEN;
	WCHAR szWindowTitle[UNLEN + ARRAYSIZE(szAppTitle) + 4] = TEXT("");

	// Get user and domain name
	GetUserNameEx(NameSamCompatible, szUsername, &dwUsernameLen);

	// Add username to window title if settings permit
	StringCchCopy(szWindowTitle, ARRAYSIZE(szAppTitle), szAppTitle);

	if (bShowUsername)
	{
		StringCchCat(szWindowTitle, ARRAYSIZE(szWindowTitle), TEXT(" - "));
		StringCchCat(szWindowTitle, ARRAYSIZE(szWindowTitle), szUsername);
	}

	SetWindowText(hWndProgMgr, (LPCWSTR)&szWindowTitle);

	return;
}
