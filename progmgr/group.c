/* * * * * * * *\
	GROUP.C -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Group window and program group/item functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <strsafe.h>
#include <uxtheme.h>
#include "progmgr.h"
#include "group.h"
#include "resource.h"
#include "registry.h"

/* Variables */
WNDCLASSEX wcGrp;
WCHAR szGrpClass[16];
HWND hWndMDIClient = NULL;
HWND hWndFoundGroup = NULL;

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
	ccs.hWindowMenu = GetSubMenu(GetMenu(g_hWndProgMgr), 2);
	ccs.idFirstChild = IDM_WINDOW_CHILDSTART;

	GetClientRect(g_hWndProgMgr, &rcFrame);

	if ((hWndMDIClient = CreateWindowEx(WS_EX_COMPOSITED,
		TEXT("MDIClient"), NULL, WS_CLIPCHILDREN | WS_CHILD |
		WS_VSCROLL | WS_HSCROLL | WS_VISIBLE,
		rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.bottom,
		g_hWndProgMgr, (HMENU)1, g_hAppInstance, (LPWSTR)&ccs)) == NULL)
	{
		return FALSE;
	}

	// Load the group class string
	LoadString(g_hAppInstance, IDS_GRPCLASS,
		szGrpClass, ARRAYSIZE(szGrpClass));

	// Register the group window class
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.style = CS_DBLCLKS;
	wce.lpfnWndProc = GroupWndProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = g_hAppInstance;
	wce.hIcon = g_hGroupIcon = LoadImage(g_hAppInstance,
		MAKEINTRESOURCE(IDI_PROGGRP), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wce.hCursor = LoadCursor(NULL, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = szGrpClass;

	if (!RegisterClassEx(&wce))
		return FALSE;
	
	// Now we can create the groups
	if(LoadConfig(FALSE, FALSE, TRUE) != RCE_SUCCESS)
		return FALSE;

	// TODO: if loading groups fails, throw an error

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

	// error checking, if we get trash don't try to
	// make a group out of it
	// TODO: if a group is garbled, throw an error
	// and ask the user if they want to delete it!
	if (pg->dwSignature != GRP_SIGNATURE)
		return NULL;

	// allocate memory for a new group
	pGroup = (PGROUP)malloc(CalculateGroupMemory(pg, 1, 0));
	if (pGroup == NULL)
		return NULL;

	// clean up the memory if it's valid
	ZeroMemory(pGroup, sizeof(*pGroup));

	// copy over the group structure
	*pGroup = *pg;

	// TODO: get group minimized/maximized flags

	mcs.szClass = szGrpClass;
	mcs.szTitle = pg->szName;
	mcs.hOwner = g_hAppInstance;
	if ((pg->rcGroup.left == CW_USEDEFAULT) & (pg->rcGroup.right == CW_USEDEFAULT))
	{
		mcs.x = mcs.y = mcs.cx = mcs.cy = CW_USEDEFAULT;
	}
	mcs.style = WS_VISIBLE | WS_THICKFRAME | WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
	// TODO: should I pass the pointer to the group through here
	// or is it better and easier to just do it with GWLP_USERDATA?
	mcs.lParam = (LPARAM)NULL;

	if ((hWndGroup = (HWND)SendMessage(hWndMDIClient, WM_MDICREATE, 0,
		(LPARAM)(LPTSTR)&mcs)) == NULL)
		return NULL;

	// Associate the group structure pointer to the group window
	SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);

	// Resize the group
	if (pg->wp.length == sizeof(WINDOWPLACEMENT))
		SetWindowPlacement(hWndGroup, &pg->wp);

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

	// Get the group window rect
	GetClientRect(hWndGroup, &rcGroupWindow);

	// Create the group window ListView control
	if ((hWndListView = CreateWindowEx(WS_EX_LEFT, WC_LISTVIEW,
		TEXT("ListView"),
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		LVS_ICON | LVS_SINGLESEL | LVS_AUTOARRANGE,
		mcs.x, mcs.y, mcs.cx, mcs.cy,
		hWndGroup, NULL, g_hAppInstance, NULL)) == NULL)
		return NULL;

	// ((LVS_AUTOARRANGE & bAutoArrange) * LVS_AUTOARRANGE)

	// Resize it to fit the window
	SetWindowPos(hWndListView, NULL,
		rcGroupWindow.left, rcGroupWindow.top,
		rcGroupWindow.right - rcGroupWindow.left,
		rcGroupWindow.bottom - rcGroupWindow.top, SWP_NOZORDER);

	// Add the explorer style because it looks good
	SetWindowTheme(hWndListView, TEXT("Explorer"), NULL);

	// Get this bad boy an image list
	// TODO: if cxicon/cyicon change make sure we handle that
	// don't require a program restart, DPI!!!
	hImageList = ImageList_Create(GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CYICON), ILC_COLOR32, 0, 1);
	ListView_SetImageList(hWndListView, hImageList, LVSIL_NORMAL);

	// since the list is viewing we can load items if applicable
	if (pGroup->cItemArray > 0)
		LoadItems(hWndGroup);

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
	PGROUP pGroup = NULL;
	PGROUP pNewGroup = NULL;
	PITEM pItem = NULL;
	HWND hWndListView = NULL;
	HIMAGELIST hImageList = NULL;
	HICON hIcon = NULL;
	LVITEM lvi = { 0 };

	// we actually just want the group pointer lol
	pGroup = (PGROUP)GetWindowLongPtr(hWndGroup, GWLP_USERDATA);

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

	// if we reallocate memory then send the new pointer in
	pNewGroup = realloc(pGroup, CalculateGroupMemory(pGroup, 1, 0));
	if (pNewGroup != NULL)
	{
		pGroup = pNewGroup;
		SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);
	}

	// add the item
	pItem = pGroup->pItemArray + pGroup->cItemArray;
	*pItem = *pi;

	// increment the item counter
	pGroup->cItemArray++;

	// then get the pointer to the group's image list
	hImageList = ListView_GetImageList(hWndListView, LVSIL_NORMAL);

	// extract that icon son!!
	hIcon = ExtractIcon(g_hAppInstance, (LPWSTR)pItem->szIconPath, pItem->iIconIndex);

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

	// this is all terrible i'm gonna rewrite this whole file
	// why is the listview even touched here just... omg...
	// BAD!! bad functions

	// TODO: fail if the listview item isn't added
	return pItem;
}

/* * * *\
	LoadItems -
		Loads all of the items in a group structure
	RETURNS -
		TRUE if successful
		FALSE otherwise
\* * * */
BOOL LoadItems(_In_ HWND hWndGroup)
{
	PGROUP pGroup = NULL;
	PGROUP pNewGroup = NULL;
	PITEM pItem = NULL;
	HWND hWndListView = NULL;
	HIMAGELIST hImageList = NULL;
	LVITEM lvi = { 0 };
	UINT cItemIndex = 0;
	HICON hIcon = NULL;

	// return NULL if we can't get to the group
	if (hWndGroup == NULL)
		return FALSE;

	// retrieve the group pointer
	pGroup = (PGROUP)GetWindowLongPtr(hWndGroup, GWLP_USERDATA);
	if (pGroup == NULL)
		return FALSE;
	
	// get the listview window
	hWndListView = FindWindowEx(hWndGroup, NULL, WC_LISTVIEW, NULL);
	if (hWndListView == NULL)
		return FALSE;

	// make sure we have enough memory for the items
	pNewGroup = realloc(pGroup, CalculateGroupMemory(pGroup, 0, 1));
	if (pNewGroup != NULL)
	{
		pGroup = pNewGroup;
		SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);
	}
	
	// then get the pointer to the group's image list
	hImageList = ListView_GetImageList(hWndListView, LVSIL_NORMAL);

	while (pGroup->cItemArray > cItemIndex)
	{
		pItem = pGroup->pItemArray + cItemIndex;

		// extract that icon son!!
		hIcon = ExtractIcon(g_hAppInstance, (LPWSTR)pItem->szIconPath, pItem->iIconIndex);

		// populate the listview with the relevant information
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iItem = cItemIndex;
		lvi.iSubItem = 0;
		lvi.pszText = pItem->szName;
		lvi.cchTextMax = ARRAYSIZE(pItem->szName);
		lvi.iImage = ImageList_AddIcon(hImageList, hIcon);
		lvi.lParam = (LPARAM)pItem;

		// copy that bad boy into the listview
		ListView_InsertItem(hWndListView, &lvi);

		cItemIndex++;
	}
	
	if (hIcon)
		// get that hicon outta here
		DestroyIcon(hIcon);

	// TODO: fail if the listview item isn't added
	return TRUE;
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
	// TODO: implement this
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
	ShellExecute(g_hWndProgMgr, TEXT("open"),
		pi->szExecPath, NULL,
		(pi->szWorkPath != TEXT("")) ? pi->szWorkPath : NULL,
		SW_NORMAL);

	// TODO: if item has run as admin as a flag,
	// run it as an admin

	// TODO: if path is invalid or other error then tell the user

	// TODO: make sure environment is
	// refreshed before execution
	return FALSE;
}

/* * * *\
	UpdateGroup -
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
	DWORD dwFlags = 0;
	HWND hWndGroup = NULL;

	// Set the important flags
	pg->dwSignature = GRP_SIGNATURE;
	pg->wVersion = GRP_VERSION;

	// Set the group checksum
	pg->wChecksum = 1; // NOTE: implement this for real later lol

	// TODO: group flags
	// pg->dwFlags = GRP_FLAG_MAXIMIZED;// GetGroupFlags(pgw);

	// Set FILETIME
	GetSystemTimeAsFileTime(&pg->ftLastWrite);
	
	if (hWndGroup = GetHwndFromPGroup(pg))
	{
		// Get the group window rect and name
		pg->wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hWndGroup, &pg->wp);
		GetWindowText(hWndGroup, pg->szName, ARRAYSIZE(pg->szName));
	}

	return;
}

/* * * *\
	GetHwndFromPGroup -
		In goes a PGROUP out comes a HWND
	RETURNS -
		HWND.
\* * * */
HWND GetHwndFromPGroup(_In_ PGROUP pg)
{
	EnumChildWindows(g_hWndProgMgr, (WNDENUMPROC)GetHwndFromPGroupEnum, (LPARAM)pg);

	return hWndFoundGroup;
}

/* * * *\
	GetHwndFromPGroupEnum -
		Enum Function
	RETURNS -
		HWND.
\* * * */
BOOL GetHwndFromPGroupEnum(_In_ HWND hwnd, _In_ LPARAM lParam)
{
	if (lParam == GetWindowLongPtr(hwnd, GWLP_USERDATA))
	{
		hWndFoundGroup = hwnd;
		return FALSE;
	}
	else
	{
		hWndFoundGroup = NULL;
		return TRUE;
	}
}

/* * * *\
	VerifyGroup -
		Verifies that a group contains the
		correct information about itself.
	ABSTRACT - 
		This function will check the checksum,
		verify the number of items, check other
		parts of the strucutre. If requested,
		it will repair the group if it is damaged.
	RETURNS -
		TRUE if group appears to be fine
		FALSE if the group is damaged
\* * * */
BOOL VerifyGroup(_In_ PGROUP pg, _In_ BOOL bRepair)
{
	if (pg->dwSignature != GRP_SIGNATURE)
		return FALSE;

	// if (pg->wVersion != GRP_VERSION)
	// some sort of version difference handling

	if (pg->wChecksum == 1234)
		// TODO: calculate checksum function
		return FALSE;

	// TODO: use ftlastwrite to throw an error
	// if system clock is in the future

	// calculate the size of the group based on item of numbers

	// TODO: change to return a dword error value
	return TRUE;
}

/* * * *\
	CalculateGroupMemory -
		Calculates the memory needed by a group.
		Takes a group structure pointer and the
		number of desired items as input.
	RETURNS -
		Size in bytes that a group should take up.
\* * * */
UINT CalculateGroupMemory(_In_ PGROUP pGroup, _In_ UINT cItems, _In_ BOOL bLean)
{
	UINT cbGroupSize = 0;
	UINT cItemBlock = 0;

	// first add the size of a group strucutre
	cbGroupSize += sizeof(GROUP);

	// calculate the total amount of items wanted
	cItemBlock = pGroup->cItemArray + cItems;

	if (!bLean)
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

	switch (message)
	{

	case WM_CREATE:
	{
		CREATESTRUCT* pCreateStruct;
		MDICREATESTRUCT* pMDICreateStruct;

		// code borrowed from winprog
		pCreateStruct = (CREATESTRUCT*)lParam;
		pMDICreateStruct = (MDICREATESTRUCT*)pCreateStruct->lpCreateParams;

		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}

	case WM_SYSCOMMAND:
	{
		if (CmdProc(hWnd, wParam, lParam))
			return 0;

		return DefMDIChildProc(hWnd, message, wParam, lParam);
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
		HWND hWndListView = NULL;

		// find the listview control
		hWndListView = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL);
		if (hWndListView == NULL)
			break;

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

		default:
			break;
		}
		return DefMDIChildProc(hWnd, message, wParam, lParam);
	}

	case WM_SIZE:
	{
		HWND hWndListView = NULL;
		RECT rcGroupWindow = { 0 };

		// get the group window rect
		GetClientRect(hWnd, &rcGroupWindow);

		// find the listview control
		hWndListView = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL);
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

	return DefMDIChildProc(hWnd, message, wParam, lParam);
}

/* * * *\
	GroupCmdProc -
		Group syscommand procedure.
	RETURNS -
		Zero if nothing, otherwise returns the good stuff.
\* * * */
LRESULT CALLBACK GroupCmdProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (GET_WM_COMMAND_ID(wParam, lParam))
	{
	case IDM_FILE_OPEN:
	{
		HWND hWndListView = NULL;
		LVITEM lvi = { 0 };
		PITEM pItem = NULL;

		// find the listview control
		hWndListView = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL);
		if (hWndListView == NULL)
			break;

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

	}

	case IDM_FILE_MOVE:
		// TODO: ITEM Create dialog with other MDI groups, pick one to move item
		break;

	case IDM_FILE_COPY:
		// TODO: ITEM Copy item structure to clipboard
		break;

	case IDM_FILE_DELETE:
		// TODO: ITEM/GROUP Delete selected item, if no items selected or no items
		// exist then delete the group
		break;

	case IDM_FILE_PROPS:
		// TODO: ITEM/GROUP Open properties of selected item or if nothing selected
		// then open group properties
		break;

	default:
		return 0;
	}

	return 0;
}
