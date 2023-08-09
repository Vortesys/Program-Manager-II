/* * * * * * * *\
	GROUP.H -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program group/item structures, functions and definitons
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Includes */
#include <wtypes.h>

/* Definitions */
#define MAX_GROUPS 512
#define MAX_ITEMS 512
#define MAX_TITLE_LENGTH MAX_PATH
// Group Format Definitions
#define GRP_SIGNATURE 0x47324D50L // PM2G
// Group Flag Values (DWORD)
#define GRP_FLAG_COMMON 0x00000001
#define GRP_FLAG_READONLY 0x00000002
#define GRP_FLAG_MINIMIZED 0x00000004
#define GRP_FLAG_MAXIMIZED 0x00000008

/* Structures */
// Group format, .GRP
typedef struct _GROUP {
	// Group file header
	DWORD dwSignature; // Set to GRP_SIGNATURE
	WORD wVersion; // Group format version
	WORD wChecksum;

	// Group information
	WCHAR szGroupName[MAX_TITLE_LENGTH];
	DWORD dwFlags; // Use with GRP_FLAG_* values.
	FILETIME ftLastWrite;

	// Items
	WORD cItems; // Number of items
	PITEM iItems; // Array of items
} GROUP, * PGROUP;

// Group window information
typedef struct _GROUPWND {
	// Window information
	HWND hWndGroup;
	RECT rcGroup; // Window rectangle

	// Group information
	HANDLE hGroup; // Handle to GROUP object
} GROUPINFO, * PGROUPINFO;

// Item Structure
typedef struct _ITEM {
	// Item executable and name
	WCHAR szItemName[MAX_TITLE_LENGTH];
	WCHAR szExecPath[MAX_PATH]; // Path of the item executable
	WCHAR szWorkPath[MAX_PATH]; // Working directory

	// Icon
	WCHAR szIconPath[MAX_PATH]; // Path of the icon
	UINT uiIconIndex; // Index of the icon
} ITEM, * PITEM;

/* Function Prototypes */
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND TempCreateGroup(HWND hMDIClient);
