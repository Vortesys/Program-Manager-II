/* * * * * * * *\
	GROUP.H -
		Copyright (c) 2023 Vortesys
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
#define MAX_TITLE_LENGTH 255 // Same as the registry key name max length
// Group Format Definitions
#define GRP_SIGNATURE 0x47324D50L // PM2G
#define GRP_VERSION 2 // Increment for breaking changes, format version
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
	WCHAR szName[MAX_TITLE_LENGTH + 1];
	WCHAR szExecPath[MAX_PATH + 1]; // Path of the executable
	WCHAR szWorkPath[MAX_PATH + 1]; // Working directory
	// Item flags
	DWORD dwFlags; // Use with ITM_FLAG_* values.
	UINT uiHotkeyModifiers;
	UINT uiHotkeyVirtualKey;
	// Icon
	WCHAR szIconPath[MAX_PATH + 1];
	INT iIconIndex;
} ITEM, * PITEM;

// Group format, .GRP
typedef struct _GROUP {
	// Group file header
	DWORD dwSignature; // Set to GRP_SIGNATURE
	WORD wVersion; // Group format version
	WORD wChecksum;
	// Group information
	WCHAR szName[MAX_TITLE_LENGTH + 1];
	DWORD dwFlags; // Use with GRP_FLAG_* values.
	FILETIME ftLastWrite;
	// Window information
	RECT rcGroup;
	// Icon
	WCHAR szIconPath[MAX_PATH + 1];
	INT iIconIndex;
	// Items
	WORD cItemArray; // Number of items in pItemArray
	PITEM pItemArray; // Array of items
} GROUP, * PGROUP;

/*  Global Variables */
extern HWND			hWndMDIClient;

/*  Local Variables */

/* Function Prototypes */
BOOL InitializeGroups(VOID);
// Group Management
HWND CreateGroup(_In_ PGROUP pg);
BOOL RemoveGroup(_In_ HWND hWndGroup, _In_ BOOL bEliminate);
// Item Management
PITEM CreateItem(_In_ HWND hWndGroup, _In_ PITEM pi);
BOOL RemoveItem(_In_ PITEM pi);
// Import/export functions
GROUP SaveGroup(_In_ PGROUP pg);
// Helper functions
UINT CalculateGroupMemory(_In_ PGROUP pGroup, _In_ UINT cItems);
// Group Window
LRESULT CALLBACK GroupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
