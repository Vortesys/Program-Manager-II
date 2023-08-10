/* * * * * * * *\
	GROUP.C -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Group window and program group/item functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "group.h"
#include "resource.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <strsafe.h>

/* Variables */
WNDCLASSEX wcGrp;
WCHAR szGrpClass[16];
PGROUPWND pgwArray[];
HWND hWndMDIClient = NULL;

/* Functions */

/* * * *\
	InitializeGroups -
		Create the MDI Client window and initialize
		the group window class.
	RETURNS -
		TRUE if groups are successfully created.
		FALSE if otherwise.
\* * * */
BOOL InitializeGroups()
{
	CLIENTCREATESTRUCT ccs = { 0 };
	WNDCLASSEX wce = { 0 };

	// Create the MDI Client Window
	ccs.hWindowMenu = GetSubMenu(GetMenu(hWndProgMgr), IDM_WINDOW);
	ccs.idFirstChild = IDM_WINDOW_CHILDSTART;

	if (!(hWndMDIClient = CreateWindowEx(WS_EX_COMPOSITED, L"MDIClient",
		NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hWndProgMgr, (HMENU)1, hAppInstance, (LPWSTR)&ccs)))
	{
		return FALSE;
	}

	// Load the group class string
	LoadString(hAppInstance, IDS_GRPCLASS, szGrpClass, ARRAYSIZE(szGrpClass));

	// Register the group window Class
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.lpfnWndProc = GroupWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = hAppInstance;
	wce.hIcon = hGroupIcon = LoadImage(hAppInstance, MAKEINTRESOURCE(IDI_PROGGRP), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wce.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szGrpClass;

	if (!RegisterClassEx(&wce))
		return FALSE;

	// TempCreateGroup();

	// CreateGroupWindow(NULL);

	return TRUE;
}


/* * * *\
	TempCreateGroup -
		th
	RETURNS -
		real
\* * * */
VOID TempCreateGroup()
{
	return;
}

/* * * *\
	CreateGroupWindow -
		Create an MDI window from a group
	RETURNS -
		Group Window structure
		or NULL GROUPWND on failure
\* * * */
GROUPWND CreateGroupWindow(PGROUP pgGroup)
{
	GROUPWND gw = { NULL };
	MDICREATESTRUCT mcs = { NULL };
	HWND hGroup;

	// TODO: allocate memory from pgwArray in here

	// TODO: rethink the structure of this, when i create a group window
	// i don't have a pggroup handle. or maybe i do. think about this
	// just a little bit more, thanks!

	mcs.szClass = szGrpClass;
	mcs.szTitle = L"";
	mcs.hOwner = hAppInstance;
	mcs.x = mcs.y = mcs.cx = mcs.cy = CW_USEDEFAULT;
	mcs.style = WS_VISIBLE;
	mcs.lParam = (LPARAM)pgGroup;

	hGroup = (HWND)SendMessage(hWndMDIClient, WM_MDICREATE, 0, (LPARAM)(LPTSTR)&mcs);
	if (!hGroup)
		return gw;

	return gw;
}

/* * * *\
	GetGroupFlags -
		Retrieve the flags of a group window.
	RETURNS -
		DWORD containing the flags of the group.
		Otherwise 0xFFFFFFFF if failure.
\* * * */
DWORD GetGroupFlags(PGROUPWND pgw)
{
	HWND hWndGrp;

	// Cancel if the group window doesn't exist
	if (pgw == NULL)
		return 0xFFFFFFFF;

	// Get the window handle
	hWndGrp = pgw->hWndGroup;

	if (hWndGrp != NULL)
	{
		// NOTE: come back to this lol
		// TODO: establish better what this actually does
		// min/max flags are pulled from hwnd
		// common/readonly flags are set upon group creation
		// this means that some flags are static whereas
		// others change over time, this means that we can
		// either assume certain flags never change over time
		// or grab them every time.
		return 0xFFFFFFFF;
	}

	return 0xFFFFFFFF;
}

/* * * *\
	SaveGroup -
		Saves group from a group window
		into a GROUP structure.
	INFO - 
		This function operates under the
		assumption that the GROUP structure
		doesn't get modified over the course
		of the MDI window's life and we can't
		just grab the structure directly and
		expect it to be updated.
	RETURNS -
		Formatted GROUP structure.
		Otherwise NULL if failure.
\* * * */
GROUP SaveGroup(PGROUPWND pgw)
{
	GROUP grp = {
		.dwSignature = GRP_SIGNATURE,
		.wVersion = GRP_VERSION,
		.wChecksum = 0,
		.szGroupName = L"",
		.dwFlags = 0,
		.ftLastWrite = 0,
		.cItems = 0,
		.iItems = NULL
	};
	HWND hWndGrp;
	WCHAR szName[MAX_TITLE_LENGTH];

	// Find the group and copy it
	grp = *pgw->pGroup;

	// Get the window handle as well
	hWndGrp = pgw->hWndGroup;

	// Set the group header information
	grp.dwSignature = GRP_SIGNATURE;
	grp.wVersion = GRP_VERSION;
	grp.wChecksum = 0; // NOTE: implement this later lol

	// Copy group information
	GetWindowText(hWndGrp, szName, MAX_TITLE_LENGTH);
	StringCchCopy(grp.szGroupName, MAX_TITLE_LENGTH, szName);

	grp.dwFlags = GetGroupFlags(pgw);

	GetSystemTimeAsFileTime(&grp.ftLastWrite);

	// Save items...
	grp.cItems = 0;
	// TODO: iterate through listview to save items

	return grp;
}

/* * * *\
	GroupWndProc -
		Group window procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		break;
	}
	default:
		// GroupProcDefault:
		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
