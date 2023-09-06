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
#include <CommCtrl.h>
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
	RECT rcFrame;

	// Create the MDI Client Window
	ccs.hWindowMenu = GetSubMenu(GetMenu(hWndProgMgr), IDM_WINDOW);
	ccs.idFirstChild = IDM_WINDOW_CHILDSTART;

	GetClientRect(hWndProgMgr, &rcFrame);

	if (!(hWndMDIClient = CreateWindowEx(WS_EX_COMPOSITED, L"MDIClient",
		NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
		rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.bottom,
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

	// CreateGroupWindow(NULL);

	return TRUE;
}

/* * * *\
	CreateGroupWindow -
		Create an MDI window from a group structure
	RETURNS -
		Pointer to a group window structure
		or NULL on failure
\* * * */
PGROUPWND CreateGroupWindow(PGROUP pgGroup)
{
	GROUPWND gw = { NULL };
	MDICREATESTRUCT mcs = { NULL };
	HICON hIconLarge = NULL;
	HICON hIconSmall = NULL;
	HICON hIconTemp = NULL;
	
	// Copy group structure to our group window
	gw.grp = *pgGroup;

	// TODO: allocate memory from pgwArray in here

	// TODO: Automatically add itself to the array of
	// PGW pointers?

	// Get group minimized/maximized flags

	mcs.szClass = szGrpClass;
	mcs.szTitle = gw.grp.szName;
	mcs.hOwner = hAppInstance;
	if ((gw.grp.rcGroup.left == CW_USEDEFAULT) & (gw.grp.rcGroup.right == CW_USEDEFAULT))
	{
		mcs.x = mcs.y = mcs.cx = mcs.cy = CW_USEDEFAULT;
	}
	else
	{
		mcs.x = gw.grp.rcGroup.left;
		mcs.y = gw.grp.rcGroup.top;
		mcs.cx = gw.grp.rcGroup.right - gw.grp.rcGroup.left;
		mcs.cy = gw.grp.rcGroup.bottom - gw.grp.rcGroup.top;
	}
	mcs.style = WS_VISIBLE;
	mcs.lParam = (LPARAM)pgGroup;

	if ((gw.hWndGroup = (HWND)SendMessage(hWndMDIClient, WM_MDICREATE, 0, (LPARAM)(LPTSTR)&mcs)) == NULL)
		return NULL;

	// Load the group icon
	if (ExtractIconEx(gw.grp.szIconPath, gw.grp.iIconIndex, &hIconLarge, &hIconSmall, 1))
	{
		if (hIconTemp = (HICON)SendMessage(gw.hWndGroup, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall))
			DestroyIcon(hIconTemp);
		if (hIconTemp = (HICON)SendMessage(gw.hWndGroup, WM_SETICON, ICON_BIG, (LPARAM)hIconLarge))
			DestroyIcon(hIconTemp);
	}

	// Create the group window ListView control
	if ((gw.hWndListView = CreateWindowEx(WS_EX_LEFT, WC_LISTVIEW, L"ListView",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		| LVS_ICON | ((LVS_AUTOARRANGE & bAutoArrange) * LVS_AUTOARRANGE) | LVS_NOSORTHEADER,
		mcs.x, mcs.y, mcs.cx, mcs.cy,
		gw.hWndGroup, NULL, hAppInstance,
		NULL)) == NULL)
		return NULL;

	// TODO: make sure the groups delete their icons upon destruction!

	return &gw;
}

/* * * *\
	SetGroupFlags -
		Retrieve the flags of a group window.
	RETURNS -
		TRUE if the flag mask is applied
		FALSE if the flags aren't set
\* * * */
BOOL SetGroupFlags(PGROUPWND pgw, DWORD dwFlags)
{
	return FALSE;
}

/* * * *\
	GetGroupFlags -
		Retrieve the flags of a group window.
	RETURNS -
		TRUE if the flag mask is applied
		FALSE if the flags aren't set
\* * * */
BOOL GetGroupFlags(PGROUPWND pgw, DWORD dwFlags)
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
	ABSTRACT - 
		This function will extract all of the
		necessary information from a group window
		in order to produce a group structure.
	RETURNS -
		Formatted GROUP structure.
		Upon failure, wChecksum will be 0.
\* * * */
GROUP SaveGroup(PGROUPWND pgw)
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
	HWND hWndGrp;
	WCHAR szGroupName[MAX_TITLE_LENGTH];

	// Find the group and copy it
	grp = pgw->grp;

	// Get the window handle as well
	hWndGrp = pgw->hWndGroup;

	// Set the group checksum
	grp.wChecksum = 1; // NOTE: implement this for real later lol

	// Copy group information
	GetWindowText(hWndGrp, szGroupName, MAX_TITLE_LENGTH);
	StringCchCopy(grp.szName, MAX_TITLE_LENGTH, szGroupName);

	grp.dwFlags = GRP_FLAG_MAXIMIZED;// GetGroupFlags(pgw);

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
		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
