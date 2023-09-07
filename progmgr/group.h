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

/* Definitions */
#define MAX_GROUPS 512
#define MAX_ITEMS 512
#define MAX_TITLE_LENGTH MAX_PATH
// Group Format Definitions
#define GRP_SIGNATURE 0x47324D50L // PM2G
#define GRP_VERSION 1 // Increment for breaking changes
// Group Flag Values (DWORD)
#define GRP_FLAG_COMMON 0x00000001
#define GRP_FLAG_READONLY 0x00000002
#define GRP_FLAG_MINIMIZED 0x00000004
#define GRP_FLAG_MAXIMIZED 0x00000008
// Item Flag Values (DWORD)
#define ITM_FLAG_MINIMIZED 0x00000001
#define ITM_FLAG_MAXIMIZED 0x00000002

/* Structures */
// Item Structure
typedef struct _ITEM {
	// Item executable and name
	WCHAR szName[MAX_TITLE_LENGTH];
	WCHAR szExecPath[MAX_PATH]; // Path of the executable
	WCHAR szWorkPath[MAX_PATH]; // Working directory
	// Item flags
	DWORD dwFlags; // Use with ITM_FLAG_* values.
	UINT uiHotkeyModifiers;
	UINT uiHotkeyVirtualKey;
	// Icon
	WCHAR szIconPath[MAX_PATH];
	INT iIconIndex;
} ITEM, * PITEM;

// Group format, .GRP
typedef struct _GROUP {
	// Group file header
	DWORD dwSignature; // Set to GRP_SIGNATURE
	WORD wVersion; // Group format version
	WORD wChecksum;
	// Group information
	WCHAR szName[MAX_TITLE_LENGTH];
	DWORD dwFlags; // Use with GRP_FLAG_* values.
	FILETIME ftLastWrite;
	// Window information
	RECT rcGroup;
	// Icon
	WCHAR szIconPath[MAX_PATH];
	INT iIconIndex;
	// Items
	WORD cItems; // Number of items
	PITEM iItems; // Array of items
} GROUP, * PGROUP;

/*  Global Variables */
extern HWND			hWndMDIClient;

/*  Local Variables */

/* Function Prototypes */
BOOL InitializeGroups();
// Group Window
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND CreateGroupWindow(GROUP grp);
// Import/export functions
GROUP SaveGroup(PGROUP pg);
