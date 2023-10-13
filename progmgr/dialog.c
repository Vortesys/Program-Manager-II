/* * * * * * * *\
	DIALOG.C -
		Copyright (c) 2023 Vortesys
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
#include <pathcch.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <strsafe.h>

/* Variables */
WCHAR		szDlgTitle[64];

/* Functions */

/* * * *\
	NewGroupDlgProc -
		Dialog procedure for creating a group.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK NewGroupDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static GROUP grp = {
		.dwSignature = GRP_SIGNATURE,
		.wVersion = GRP_VERSION,
		.wChecksum = 0,
		.szName = TEXT(""),
		.dwFlags = 0,
		.ftLastWrite = 0,
		.szIconPath = TEXT(""),
		.iIconIndex = 0,
		.cItemArray = 0,
		.pItemArray = 0
	};
	BOOL bOKEnabled = FALSE;
	WCHAR szBuffer[MAX_TITLE_LENGTH] = { TEXT("\0") };
	HICON hIconDef = NULL;
	HICON hIconDlg = NULL;

	switch (message)
	{

	case WM_INITDIALOG:
		// TODO:
		// Enable creation of common groups (enable the controls)
		// if permissions are available.
		// TODO:
		// fix minor GDI font/region leak

		// Reset the group structure
		grp.dwSignature = GRP_SIGNATURE;
		grp.wVersion = GRP_VERSION;
		grp.wChecksum = 0;
		grp.dwFlags = 0;
		grp.cItemArray = 0;
		// grp.pItemArray = 0; TODO: idk make sure this ain't all screwed up

		// Set the window title
		LoadString(hAppInstance, IDS_DLT_GRP_NEW, szDlgTitle, ARRAYSIZE(szDlgTitle));
		SetWindowText(hWndDlg, szDlgTitle);
		
		// Populate the icon with the default path and index.
		GetModuleFileName(hAppInstance, (LPWSTR)&grp.szIconPath, ARRAYSIZE(grp.szIconPath));
		grp.iIconIndex = IDI_PROGGRP - 1;

		// Get the default hIcon so we can delete it later
		hIconDef = (HICON)SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_GETICON, 0, 0);

		// Set the icon in the dialog
		hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)&grp.szIconPath, grp.iIconIndex);
		SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);

		// Set the maximum input length of the text boxes
		SendDlgItemMessage(hWndDlg, IDD_NAME, EM_LIMITTEXT, MAX_TITLE_LENGTH, 0);

		// Disable the OK button since we're starting with no text in the box.
		EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);

		// Enable the common group checkbox if perms are good
		EnableWindow(GetDlgItem(hWndDlg, IDD_COMMGROUP), bPermAdmin);

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
			if (PickIconDlg(hWndDlg, (LPWSTR)&grp.szIconPath, ARRAYSIZE(grp.szIconPath), &grp.iIconIndex) == TRUE)
			{
				// Since we've got the new icon...
				hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)&grp.szIconPath, grp.iIconIndex);
				SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);
			}

			break;

		case IDD_OK:
			// Check that all the applicable fields are filled out,
			// and if not then set the focus to the offending field
			if (!(bOKEnabled = GetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)&szBuffer, ARRAYSIZE(szBuffer))))
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

				// Set FILETIME
				GetSystemTimeAsFileTime(&grp.ftLastWrite);

				// Set the rectangle of the group to be CW_USEDEFAULT
				grp.rcGroup.left = grp.rcGroup.top = grp.rcGroup.right = grp.rcGroup.bottom = CW_USEDEFAULT;

				// Group's ready!
				if (CreateGroup(&grp) != NULL)
				{
					EndDialog(hWndDlg, FALSE);
					break;
				}
					
				// Failure!
				EndDialog(hWndDlg, FALSE);
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
	static ITEM itm = {
		.szName = TEXT(""),
		.szExecPath = TEXT(""),
		.szWorkPath = TEXT(""),
		.dwFlags = 0,
		.uiHotkeyModifiers = 0,
		.uiHotkeyVirtualKey = 0,
		.szIconPath = TEXT(""),
		.iIconIndex = 0,
	};
	BOOL bOKEnabled = FALSE;
	BOOL bWorkPath = FALSE;
	WCHAR szBuffer[MAX_TITLE_LENGTH] = { TEXT("\0") };
	WCHAR szPathBuffer[MAX_PATH] = { TEXT("\0") };
	HICON hIconDef = NULL;
	HICON hIconDlg = NULL;

	switch (message)
	{

	case WM_INITDIALOG:
		// TODO:
		// require a valid group to be selected or else the dialog won't
		// EVEN BE AVAILABLE!!! or let you press ok.
		
		// Reset the item structure
		itm.dwFlags = 0;
		itm.uiHotkeyModifiers = 0;
		itm.uiHotkeyVirtualKey = 0;
		
		// Set the window title
		LoadString(hAppInstance, IDS_DLT_ITEM_NEW, szDlgTitle, ARRAYSIZE(szDlgTitle));
		SetWindowText(hWndDlg, szDlgTitle);

		// Populate the icon with the default path and index.
		GetModuleFileName(NULL, (LPWSTR)&itm.szIconPath, MAX_PATH);
		itm.iIconIndex = IDI_PROGITM - 1;

		// Get the default hIcon so we can delete it later
		hIconDef = (HICON)SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_GETICON, 0, 0);

		// Set the icon in the dialog
		hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)&itm.szIconPath, itm.iIconIndex);
		SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);

		// Set the maximum input length of the text boxes
		SendDlgItemMessage(hWndDlg, IDD_NAME, EM_LIMITTEXT, MAX_TITLE_LENGTH, 0);
		SendDlgItemMessage(hWndDlg, IDD_PATH, EM_LIMITTEXT, MAX_PATH, 0);
		SendDlgItemMessage(hWndDlg, IDD_WORKPATH, EM_LIMITTEXT, MAX_PATH, 0);

		// Disable the OK button since we're starting with no text in the box.
		EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);

		break;

	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE)
		{
			if ((LOWORD(wParam) == IDD_NAME) || (LOWORD(wParam) == IDD_PATH) || (LOWORD(wParam) == IDD_WORKPATH))
			{
				// A control has changed. See what's up...
				bOKEnabled = GetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)&szBuffer, ARRAYSIZE(szBuffer));
				bOKEnabled = bOKEnabled && GetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));

				if (bWorkPath)
					bOKEnabled = bOKEnabled && GetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));

				// Check that we have selected an MDI child window as well
				if ((HWND)SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, FALSE) == (HWND)NULL)
					bOKEnabled = FALSE;

				// Enable or disable the relevant controls based on the information
				EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);
			}
		}

		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{

		case IDD_BROWSE:
		{
			OPENFILENAME ofn;
			LPVOID lpData;
			DWORD dwVerBuffer = 0;
			UINT uiTitleLen = MAX_TITLE_LENGTH;
			WCHAR szNameBuffer[MAX_TITLE_LENGTH] = { TEXT("\0") };
			WCHAR szFileBuffer[MAX_PATH] = { TEXT("\0") };

			GetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szFileBuffer, ARRAYSIZE(szFileBuffer));

			// Initialize the structure
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWndDlg;
			ofn.lpstrFilter = TEXT("Programs\0*.exe;*.bat;*.com;*.cmd;*.lnk\0All Files (*.*)\0*.*\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = (LPWSTR)&szFileBuffer;
			// ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = ARRAYSIZE(szFileBuffer);
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE)
				SetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szFileBuffer);
			else
				break;

			// copy the filename to a new buffer for later
			StringCchCopy(szNameBuffer, ARRAYSIZE(szNameBuffer), szFileBuffer);

			// let's retrieve the application's friendly name too
			dwVerBuffer = GetFileVersionInfoSize((LPCWSTR)szFileBuffer, NULL);
			// if (dwVerBuffer != 0)
			if (NULL)
			{
				lpData = malloc(dwVerBuffer);
				// TODO: totally redo this it doesn't work

				if (lpData != NULL)
					GetFileVersionInfo((LPCWSTR)szFileBuffer, (DWORD)0, dwVerBuffer, lpData);
				else
					break;
				
				// TODO: this is hardcoded to english. make it dynamic later
				// TODO: this is so busted lol, really make it dynamic so it works
				VerQueryValue(lpData,
					TEXT("\\StringFileInfo\\040904b0\\FileDescription"),
					(LPVOID)&szNameBuffer, &uiTitleLen); // == 0

				if (lpData)
					free(lpData);
			}
			else
			{
				PathStripPath(szNameBuffer);
				PathCchRemoveExtension(szNameBuffer, ARRAYSIZE(szNameBuffer));
			}

			SetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)szNameBuffer);

			// let's get the icon now
			if (ExtractIcon(hAppInstance, (LPWSTR)szFileBuffer, -1) > 0)
			{
				// file contains at least one icon, set the itm struct values
				StringCchCopy(itm.szIconPath, ARRAYSIZE(itm.szIconPath), szFileBuffer);
				itm.iIconIndex = 0;
				
				// update icon
				hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)itm.szIconPath, itm.iIconIndex);
				SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);
			}

			break;
		}

		case IDD_BROWSE2:
		{
			OPENFILENAME ofn;

			GetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));

			// Initialize the structure
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWndDlg;
			ofn.lpstrFilter = TEXT("Folders\0");
			ofn.nFilterIndex = 1;
			ofn.lpstrFile = (LPWSTR)&szPathBuffer;
			// ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = ARRAYSIZE(szPathBuffer);
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE) {
				SetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer);
			}

			break;
		}
			

		case IDD_CHICON:
			if (PickIconDlg(hWndDlg, (LPWSTR)&itm.szIconPath, ARRAYSIZE(itm.szIconPath), &itm.iIconIndex) == TRUE)
			{
				// Since we've got the new icon...
				hIconDlg = ExtractIcon(hAppInstance, (LPWSTR)itm.szIconPath, itm.iIconIndex);
				SendDlgItemMessage(hWndDlg, IDD_STAT_ICON, STM_SETICON, (WPARAM)hIconDlg, 0);
			}

			break;

		case IDD_WORKDIR:
			bWorkPath = IsDlgButtonChecked(hWndDlg, IDD_WORKDIR);

			if (bWorkPath)
			{
				GetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));
				PathCchRemoveFileSpec((PWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));
				SetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer);
			}
			else
			{
				SetWindowText(GetDlgItem(hWndDlg, IDD_WORKPATH), 0);
			}
				
			EnableWindow(GetDlgItem(hWndDlg, IDD_STAT_WORKDIR), bWorkPath);
			EnableWindow(GetDlgItem(hWndDlg, IDD_WORKPATH), bWorkPath);
			EnableWindow(GetDlgItem(hWndDlg, IDD_BROWSE2), bWorkPath);

			break;

		case IDD_OK:
			// Check that all the applicable fields are filled out,
			// and if not then set the focus to the offending field
			if (!(bOKEnabled = GetDlgItemText(hWndDlg, IDD_NAME, (LPWSTR)&szBuffer, ARRAYSIZE(szBuffer))))
				SendDlgItemMessage(hWndDlg, IDD_NAME, EM_TAKEFOCUS, 0, 0);
			if (!(bOKEnabled = GetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer))))
				SendDlgItemMessage(hWndDlg, IDD_PATH, EM_TAKEFOCUS, 0, 0);
			if (bWorkPath)
			{
				if (!(bOKEnabled = GetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer))))
					SendDlgItemMessage(hWndDlg, IDD_WORKPATH, EM_TAKEFOCUS, 0, 0);
			}
				
			// Enable or disable the OK button based on the information
			EnableWindow(GetDlgItem(hWndDlg, IDD_OK), bOKEnabled);

			if (bOKEnabled)
			{
				// Set the name of the item
				StringCchCopy(itm.szName, ARRAYSIZE(szBuffer), szBuffer);

				// And the paths...
				GetDlgItemText(hWndDlg, IDD_PATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));
				StringCchCopy(itm.szExecPath, ARRAYSIZE(szPathBuffer), szPathBuffer);
				GetDlgItemText(hWndDlg, IDD_WORKPATH, (LPWSTR)&szPathBuffer, ARRAYSIZE(szPathBuffer));
				StringCchCopy(itm.szWorkPath, ARRAYSIZE(szPathBuffer), szPathBuffer);

				// Item's ready!
				if (CreateItem((HWND)SendMessage(hWndMDIClient, WM_MDIGETACTIVE, 0, 0), &itm) != NULL)
				{
					EndDialog(hWndDlg, FALSE);
					break;
				}

				// Failure!
				EndDialog(hWndDlg, FALSE);
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
	ShutdownDlgProc -
		Dialog procedure for powering off the computer.
	RETURNS -
		TRUE if message is handled, FALSE otherwise.
\* * * */
BOOL CALLBACK ShutdownDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HICON hIconDlg = NULL;

	switch (message)
	{

	case WM_INITDIALOG:
		break;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(wParam, lParam))
		{

		case IDD_OK:
		{
		}

		case IDD_CANCEL:
			EndDialog(hWndDlg, FALSE);
			break;

		default:
			break;
		}
		break;

	default:
		// Cleanup
		if (hIconDlg)
			DestroyIcon(hIconDlg);

		return FALSE;
	}
	return TRUE;
}
