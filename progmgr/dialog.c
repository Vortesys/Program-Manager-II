/* * * * * * * *\
	DIALOG.C -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program Manager's dialogs and related
		common functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "dialog.h"
#include "group.h"
#include "resource.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Shlobj.h>
#include <strsafe.h>

/* Functions */

/* * * *\
	NewGroupDlgProc -
		Dialog procedure for creating a group.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK NewGroupDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	GROUP grp = {
		.dwSignature = GRP_SIGNATURE,
		.wVersion = GRP_VERSION,
		.wChecksum = 0,
		.szName = L"",
		.dwFlags = 0,
		.ftLastWrite = 0,
		.cItems = 0,
		.iItems = NULL
	};
	BOOL bOKEnabled = FALSE;
	WCHAR szBuffer[MAX_TITLE_LENGTH] = { L"\0" };
	HICON hIconDef = NULL;
	HICON hIconDlg = NULL;
	WCHAR szIconPath[MAX_PATH] = { L"\0" };
	INT iIconIndex = 0;

	switch (message)
	{

	case WM_INITDIALOG:
		// TODO:
		// Enable creation of common groups (enable the controls)
		// if permissions are available.
		// TODO:
		// fix minor GDI font/region leak
		
		// Populate the icon with the default path and index.
		GetModuleFileName(NULL, (LPWSTR)&szIconPath, MAX_PATH);
		iIconIndex = IDI_PROGGRP - 1;

		// Get the default hIcon so we can delete it later
		hIconDef = (HICON)SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_GETICON, 0, 0);

		// Set the icon in the dialog
		hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)&szIconPath, iIconIndex);
		SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);

		// Set the maximum input length of the text boxes
		SendDlgItemMessage(hWndDlg, IDD_NAME, EM_LIMITTEXT, MAX_TITLE_LENGTH, 0);

		// Disable the OK button since we're starting with no text in the box.
		EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);

		break;

	case WM_COMMAND:

		if (HIWORD(wParam) == EN_CHANGE)
		{
			if (LOWORD(wParam) == IDD_NAME)
			{
				// Name text control changed. See what's up...
				bOKEnabled = GetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)&szBuffer, ARRAYSIZE(szBuffer));

				// Enable or disable the OK button based on the information
				EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);
			}
		}

		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{

		case IDD_CHICON:
			if (PickIconDlg(hWndDlg, (LPWSTR)&szIconPath, ARRAYSIZE(szIconPath), &iIconIndex) == TRUE)
			{
				// Since we've got the new icon...
				hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)&szIconPath, iIconIndex);
				SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);
			}

			break;

		case IDD_OK:
			// Check that all the applicable fields are filled out
			bOKEnabled = GetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)&szBuffer, ARRAYSIZE(szBuffer));

			// If not, set the focus to the offending field
			if (!bOKEnabled)
				SendDlgItemMessage(hWndDlg, IDD_NAME, EM_TAKEFOCUS, 0, 0);

			// Enable or disable the OK button based on the information
			EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);

			if (bOKEnabled)
			{
				// Set the name of the group
				StringCchCopy(grp.szName, ARRAYSIZE(szBuffer), szBuffer);

				// Set the flags of the group
				if (SendDlgItemMessage(hWndDlg, IDD_COMMGROUP, BM_GETCHECK, 0, 0) != BST_UNCHECKED)
					grp.dwFlags = grp.dwFlags || GRP_FLAG_COMMON;

				// TODO: set FILETIME

				// Set the rectangle of the group to be CW_USEDEFAULT
				grp.rcGroup.left = grp.rcGroup.top = grp.rcGroup.right = grp.rcGroup.bottom = CW_USEDEFAULT;
				
				// Set icon properties
				StringCchCopy(grp.szIconPath, ARRAYSIZE(szIconPath), szIconPath);
				grp.iIconIndex = iIconIndex;

				// Group's ready!
				if (CreateGroupWindow(&grp) != NULL)
					break;

				// Failure!
				break;
			}

			break;

		case IDD_CANCEL:
			EndDialog(hWndDlg, FALSE);
			break;

		default:
			break;
		}

	default:
		// Cleanup
		if (hIconDef)
			DestroyIcon(hIconDef);
		if (hIconDlg)
			DestroyIcon(hIconDlg);

		return FALSE;
	}

	return TRUE;
}

/* * * *\
	NewItemDlgProc -
		Dialog procedure for creating or modifying
		an item.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK NewItemDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_INITDIALOG:
		// Place message cases here. 
		break;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{

		case IDD_CANCEL:
			EndDialog(hWndDlg, FALSE);
			break;

		default:
			break;
		}

	default:
		return FALSE;
	}

	return TRUE;
}
