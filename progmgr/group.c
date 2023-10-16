/* * * * * * * *\
	GROUP.C -
		Copyright (c) 2023 Vortesys
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
#include <windowsx.h>
#include <CommCtrl.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <uxtheme.h>

/* Variables */
WNDCLASSEX wcGrp;
WCHAR szGrpClass[16];
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
BOOL InitializeGroups(VOID)
{
	CLIENTCREATESTRUCT ccs = { 0 };
	WNDCLASSEX wce = { 0 };
	RECT rcFrame;

	// Create the MDI Client Window
	ccs.hWindowMenu = GetSubMenu(GetMenu(hWndProgMgr), 2);
	ccs.idFirstChild = IDM_WINDOW_CHILDSTART;

	GetClientRect(hWndProgMgr, &rcFrame);

	if ((hWndMDIClient = CreateWindowEx(WS_EX_COMPOSITED,
		TEXT("MDIClient"), NULL, WS_CLIPCHILDREN | WS_CHILD |
		WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
		rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.bottom,
		hWndProgMgr, (HMENU)1, hAppInstance, (LPWSTR)&ccs)) == NULL)
	{
		return FALSE;
	}

	// Load the group class string
	LoadString(hAppInstance, IDS_GRPCLASS,
		szGrpClass, ARRAYSIZE(szGrpClass));

	// Register the group window class
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = CS_DBLCLKS;
	wce.lpfnWndProc = GroupWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = hAppInstance;
	wce.hIcon = hGroupIcon = LoadImage(hAppInstance,
		MAKEINTRESOURCE(IDI_PROGGRP), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wce.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szGrpClass;

	if (!RegisterClassEx(&wce))
		return FALSE;

	// TODO: go thru registry and load all saved groups here

	return TRUE;
}

/* * * *\
	CreateGroup -
		Create an MDI window from a group structure
	RETURNS -
		Handle to the new group window
		or NULL on failure
\* * * */
HWND CreateGroup(_In_ PGROUP pg)
{
	MDICREATESTRUCT mcs = { NULL };
	PGROUP pGroup = NULL;
	HICON hIconLarge = NULL;
	HICON hIconSmall = NULL;
	HICON hIconTemp = NULL;
	HWND hWndGroup = NULL;
	HIMAGELIST hImageList = NULL;
	HWND hWndListView = NULL;
	RECT rcGroupWindow = { 0 };
	LVCOLUMN lvc = { 0 };

	if (pg == NULL)
		return NULL;

	// allocate memory for a new group
	pGroup = (PGROUP)malloc(CalculateGroupMemory(pg, 0));

	if (pGroup == NULL)
		return NULL;

	// clean up the memory if it's valid
	ZeroMemory(pGroup, CalculateGroupMemory(pg, 0));

	// copy over the group structure
	*pGroup = *pg;

	// TODO: get group minimized/maximized flags

	mcs.szClass = szGrpClass;
	mcs.szTitle = pg->szName;
	mcs.hOwner = hAppInstance;
	if ((pg->rcGroup.left == CW_USEDEFAULT) & (pg->rcGroup.right == CW_USEDEFAULT))
	{
		mcs.x = mcs.y = mcs.cx = mcs.cy = CW_USEDEFAULT;
	}
	else
	{
		mcs.x = pg->rcGroup.left;
		mcs.y = pg->rcGroup.top;
		mcs.cx = pg->rcGroup.right - pg->rcGroup.left;
		mcs.cy = pg->rcGroup.bottom - pg->rcGroup.top;
	}
	mcs.style = WS_VISIBLE;
	// TODO: should I pass the pointer to the group through here
	// or is it better and easier to just do it with GWLP_USERDATA?
	mcs.lParam = (LPARAM)NULL;

	if ((hWndGroup = (HWND)SendMessage(hWndMDIClient, WM_MDICREATE, 0,
		(LPARAM)(LPTSTR)&mcs)) == NULL)
		return NULL;

	// Associate the group structure pointer to the group window
	SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);

	// Load the group icon
	if (ExtractIconEx(pg->szIconPath, pg->iIconIndex, &hIconLarge, &hIconSmall, 1))
	{
		if (hIconTemp = (HICON)SendMessage(hWndGroup, WM_SETICON,
			ICON_SMALL, (LPARAM)hIconSmall))
			DestroyIcon(hIconTemp);
		if (hIconTemp = (HICON)SendMessage(hWndGroup, WM_SETICON,
			ICON_BIG, (LPARAM)hIconLarge))
			DestroyIcon(hIconTemp);
	}

	// get the group window rect
	GetClientRect(hWndGroup, &rcGroupWindow);

	// Create the group window ListView control
	if ((hWndListView = CreateWindowEx(WS_EX_LEFT, WC_LISTVIEW,
		TEXT("ListView"),
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		LVS_ICON | LVS_SINGLESEL | LVS_AUTOARRANGE,
		mcs.x, mcs.y, mcs.cx, mcs.cy,
		hWndGroup, NULL, hAppInstance,
		NULL)) == NULL)
		return NULL;

	// ((LVS_AUTOARRANGE & bAutoArrange) * LVS_AUTOARRANGE)

	// resize it to fit the window
	SetWindowPos(hWndListView, NULL,
		rcGroupWindow.left, rcGroupWindow.top,
		rcGroupWindow.right - rcGroupWindow.left,
		rcGroupWindow.bottom - rcGroupWindow.top, SWP_NOZORDER);

	// add the explorer style because it looks good
	SetWindowTheme(hWndListView, TEXT("Explorer"), NULL);

	// get this bad boy an image list
	// TODO: if cxicon/cyicon change make sure we handle that
	// don't require a program restart
	hImageList = ImageList_Create(GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CYICON), ILC_COLOR32, 0, 1);
	ListView_SetImageList(hWndListView, hImageList, LVSIL_NORMAL);

	// TODO: make sure the groups delete their icons upon destruction!
	// AND IMAGE LIST!!!!!!!!? i think it's nuked w/ listview though
	return hWndGroup;
}

/* * * *\
	RemoveGroup -
		Removes and cleans up a group window
	RETURNS -
		TRUE if cleanup is successful,
		FALSE otherwise.
\* * * */
BOOL RemoveGroup(_In_ HWND hWndGroup, _In_ BOOL bEliminate)
{
	PGROUP pGroup = NULL;
	BOOL bReturn = TRUE;

	// TODO: do i have to delete the titlebar icons?

	if (bEliminate == TRUE)
	{
		// TODO: remove group from registry
		// if successful blah blah blah
		bEliminate = TRUE;
	}

	// get the group struct pointer
	if ((pGroup = (PGROUP)GetWindowLongPtr(hWndGroup, GWLP_USERDATA)) == NULL)
		bReturn = FALSE;
	else
		free(pGroup);

	// close the group window
	SendMessage(hWndMDIClient, WM_MDIDESTROY, (WPARAM)hWndGroup, 0);

	return bReturn;
}

/* * * *\
	CreateItem -
		Creates a new program item
	RETURNS -
		Pointer to the item if successful
		NULL otherwise
\* * * */
PITEM CreateItem(_In_ HWND hWndGroup, _In_ PITEM pi)
{
	HIMAGELIST hImageList = NULL;
	HICON hIcon = NULL;
	LVITEM lvi = { 0 };
	PGROUP pGroup = NULL;
	PITEM pItem = NULL;
	UINT uiTest = 0;
	HWND hWndListView = NULL;

	// we actually just want the group pointer lol
	pGroup = (PGROUP)GetWindowLongPtr(hWndGroup, GWLP_USERDATA);
	uiTest = pGroup->cItemArray;

	// return NULL if we can't get to the group or item
	if (hWndGroup == NULL)
		return NULL;
	if (pGroup == NULL)
		return NULL;
	if (pi == NULL)
		return NULL;

	// get the listview window
	hWndListView = FindWindowEx(hWndGroup, NULL, WC_LISTVIEW, NULL);
	if (hWndListView == NULL)
		return NULL;

	// compare group's existing memory to needed memory
	// if needed, reallocate the group's memory
	if (_msize(pGroup) != CalculateGroupMemory(pGroup, 1))
	{
		// if we reallocate memory then send the new pointer in
		PGROUP pNewGroup = realloc(pGroup, CalculateGroupMemory(pGroup, 1));
		if (pNewGroup != NULL) {
			pGroup = pNewGroup;
			SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);
		}
	}

	// add the item
	pItem = pGroup->pItemArray + pGroup->cItemArray;
	*pItem = *pi;

	// increment the item counter
	pGroup->cItemArray++;

	// then get the pointer to the group's image list
	hImageList = ListView_GetImageList(hWndListView, LVSIL_NORMAL);

	// extract that icon son!!
	hIcon = ExtractIcon(hAppInstance, (LPWSTR)pItem->szIconPath, pItem->iIconIndex);

	// populate the listview with the relevant information
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvi.iItem = pGroup->cItemArray;
	lvi.iSubItem = 0;
	lvi.pszText = pItem->szName;
	lvi.cchTextMax = ARRAYSIZE(pItem->szName);
	lvi.iImage = ImageList_AddIcon(hImageList, hIcon);
	lvi.lParam = (LPARAM)pItem;

	// copy that bad boy into the listview
	ListView_InsertItem(hWndListView, &lvi);

	// get that hicon outta here
	DestroyIcon(hIcon);

	// TODO: fail if the listview item isn't added
	return pItem;
}

/* * * *\
	RemoveItem -
		Removes a program item
	RETURNS -
		TRUE if item was successfully removed
		FALSE otherwise
\* * * */
BOOL RemoveItem(_In_ PITEM pi)
{
	// return NULL if we can't get to the group
	if (pi == NULL)
		return FALSE;

	return FALSE;
}

/* * * *\
	ExecuteItem -
		Runs a program item
	RETURNS -
		TRUE if item was successfully ran
		FALSE otherwise
\* * * */
BOOL ExecuteItem(_In_ PITEM pi)
{
	// TODO: nCmdShow from program item flags
	ShellExecute(hWndProgMgr, TEXT("open"),
		pi->szExecPath, NULL,
		(pi->szWorkPath != TEXT("")) ? pi->szWorkPath : NULL,
		SW_NORMAL);

	// TODO: if item has run as admin as a flag,
	// run it as an admin

	// TODO: make sure environment is
	// refreshed before execution
	return FALSE;
}

/* * * *\
	SaveGroup -
		Updates group information to prepare
		it for being saved.
	ABSTRACT - 
		This function will update all of the
		relevant structures in a group struct
		in preparation for saving the group.
	RETURNS -
		Nothing.
\* * * */
VOID UpdateGroup(_In_ PGROUP pg)
{
	GROUP grp = {
		.dwSignature = GRP_SIGNATURE,
		.wVersion = GRP_VERSION,
		.wChecksum = 0,
		.szName = TEXT(""),
		.dwFlags = 0,
		.ftLastWrite = 0,
		.cItemArray = 0,
		.pItemArray = 0
	};

	// Set the group checksum
	pg->wChecksum = 1; // NOTE: implement this for real later lol

	// TODO: set group flags
	// pg->dwFlags = GRP_FLAG_MAXIMIZED;// GetGroupFlags(pgw);

	// Set FILETIME
	GetSystemTimeAsFileTime(&pg->ftLastWrite);

	return;
}

/* * * *\
	CalculateGroupMemory -
		Calculates the memory needed by a group.
		Takes a group structure pointer and the
		number of desired items as input.
	RETURNS -
		Size in bytes that a group should take up.
\* * * */
UINT CalculateGroupMemory(_In_ PGROUP pGroup, _In_ UINT cItems)
{
	UINT cbGroupSize = 0;
	UINT cItemBlock = 0;

	// first add the size of a group strucutre
	cbGroupSize += sizeof(GROUP);

	// calculate the total amount of items wanted, set
	// to 16 if there's less than 16 items so we always
	// have some memory ready
	cItemBlock = pGroup->cItemArray + cItems;

	// round the amount of items to the nearest but highest ITEM_BATCH_COUNT
	cItemBlock = ((cItemBlock + ITEM_BATCH_COUNT) / ITEM_BATCH_COUNT) * ITEM_BATCH_COUNT;

	// finally calculate the total group size
	cbGroupSize += cItemBlock * sizeof(ITEM);

	return cbGroupSize;
}

/* * * *\
	GroupWndProc -
		Group window procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hWndListView = NULL;

	// find the listview control
	hWndListView = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL);

	switch (message)
	{

	case WM_CREATE:
	{
		CREATESTRUCT* pCreateStruct;
		MDICREATESTRUCT* pMDICreateStruct;

		// code borrowed from winprog
		pCreateStruct = (CREATESTRUCT*)lParam;
		pMDICreateStruct = (MDICREATESTRUCT*)pCreateStruct->lpCreateParams;

		return 0;
	}

	case WM_CONTEXTMENU:
	{
		POINT pt = { 0 };
		HMENU hMenu = NULL;
		BOOL bPopup = FALSE;

		// LVHT_ONITEM
		// LVHT_NOWHERE

		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		break;
	}

	case WM_CLOSE:
		return ShowWindow(hWnd, SW_MINIMIZE);

	case WM_NOTIFY:
	{
		LPNMHDR lpnmhdr = (LPNMHDR)lParam;

		switch (lpnmhdr->code)
		{

		case NM_DBLCLK:
		{
			LVITEM lvi = { 0 };
			PITEM pItem = NULL;

			// what info do we want?
			lvi.mask = LVIF_PARAM;
			lvi.iItem = ((LPNMITEMACTIVATE)lParam)->iItem;

			// get the listview item
			ListView_GetItem(hWndListView, &lvi);

			// get the item pointer
			pItem = (PITEM)lvi.lParam;

			// verify...
			if (pItem)
			{
				// and execute!
				ExecuteItem(pItem);
			}

			break;
		}

		}
		return 0;
	}

	case WM_SIZE:
	{
		HWND hWndListView = NULL;
		RECT rcGroupWindow = { 0 };

		// get the group window rect
		GetClientRect(hWnd, &rcGroupWindow);
		if (hWndListView == NULL)
			break;
		
		// resize it to fit the window
		SetWindowPos(hWndListView, NULL,
			rcGroupWindow.left, rcGroupWindow.top,
			rcGroupWindow.right - rcGroupWindow.left,
			rcGroupWindow.bottom - rcGroupWindow.top, SWP_NOZORDER);

		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}
		
	default:
		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}

	return TRUE;
}
