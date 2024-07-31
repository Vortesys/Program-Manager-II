/* * * * * * * *\
	PROGMGR.C -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Program Manager's main file, now with extra wWinMain!
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "group.h"
#include "resource.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#define SECURITY_WIN32
#include <Windows.h>

/* Variables */
// Global
#ifdef _DEBUG
BOOL		bIsDebugBuild = TRUE;
#else
BOOL		bIsDebugBuild = FALSE;
#endif
BOOL		bIsDefaultShell = FALSE;
// Handles
HINSTANCE	hAppInstance;
HANDLE		hAppHeap;
HWND		hWndProgMgr = NULL;
// Icons
HICON		hProgMgrIcon = NULL;
HICON		hGroupIcon = NULL;
// Global Strings
WCHAR		szAppTitle[64];
WCHAR		szProgMgr[] = TEXT("progmgr");
WCHAR		szWebsite[64];
WCHAR		szClass[16];
// Permissions
BOOL		bPermAdmin; // Has Administrator permissions
BOOL		bPermGuest; // Has Guest permissions
BOOL		bPermPower; // Has power option permissions

/* Functions */

/* * * *\
	UpdatePermissions -
		Update the permissions for the user based
		on account settings.
	RETURNS -
		TRUE if permissions are found successfully.
		FALSE if otherwise.
\* * * */
BOOL UpdatePermissions(VOID)
{
	// SE_SHUTDOWN_NAME
	bPermAdmin = FALSE;
	bPermGuest = FALSE;
	bPermPower = FALSE;
	return FALSE;
}

/* * * *\
	wWinMain -
		Program Manager's entry point.
\* * * */
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	MSG msg = { 0 };
	HANDLE hAccel;
	HMENU hMenu;
	HMENU hSystemMenu;
	WNDCLASS wc = { 0 };
	WCHAR szBuffer[MAX_PATH];
	RECT rcRoot;
	POINT ptOffset = { 0 };

	// Initialize the instance
	hAppInstance = hInstance;

	// Create our global heap handle
	hAppHeap = GetProcessHeap();

	// Create Strings
	LoadString(hAppInstance, IDS_PMCLASS, szClass, ARRAYSIZE(szClass));
	LoadString(hAppInstance, IDS_APPTITLE, szAppTitle, ARRAYSIZE(szAppTitle));
	LoadString(hAppInstance, IDS_WEBSITE, szWebsite, ARRAYSIZE(szWebsite));

	// Get Desktop background color
	//CreateSolidBrush(GetBackgroundColor

	// Register the Frame Window Class
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hAppInstance;
	wc.hIcon = hProgMgrIcon = LoadImage(hAppInstance, MAKEINTRESOURCE(IDI_PROGMGR), IMAGE_ICON,
		0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
	wc.lpszClassName = szClass;

	if (!RegisterClass(&wc))
		return FALSE;

	// Load the Accelerator table
	hAccel = LoadAccelerators(hAppInstance, MAKEINTRESOURCE(IDA_ACCELS));
	if (!hAccel)
		return FALSE;

	// Perform Registry actions, close if registry is inaccessible.
	if (!InitializeRegistryKeys())
		return FALSE;
	
	bIsDefaultShell = IsProgMgrDefaultShell();

	// Load configuration, but don't load groups yet
	if(LoadConfig(TRUE, TRUE, FALSE) != RCE_SUCCESS)
		return FALSE;

	// Get size of the root HWND
	GetWindowRect(GetDesktopWindow(), &rcRoot);

	// Get the initial window offset
	SystemParametersInfo(SPI_ICONHORIZONTALSPACING, 0, &ptOffset.x, 0);
	SystemParametersInfo(SPI_ICONVERTICALSPACING, 0, &ptOffset.y, 0);

	// Create main window with a default size
	// TODO: i pulled 320x240 out of my ass, make this dynamic later
	if ((hWndProgMgr = CreateWindowW(wc.lpszClassName, szAppTitle,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		rcRoot.left + ptOffset.x, rcRoot.top + ptOffset.y,
		rcRoot.left + ptOffset.x + 320, rcRoot.top + ptOffset.y + 240,
		0, 0, hAppInstance, NULL)) == NULL)
		return 2;

	// Set the window size from the registry, but only if the coords make sense
	if ((rcMainWindow.left != rcMainWindow.right) && (rcMainWindow.top != rcMainWindow.bottom))
		SetWindowPos(hWndProgMgr, NULL,
			rcMainWindow.left, rcMainWindow.top,
			rcMainWindow.right - rcMainWindow.left,
			rcMainWindow.bottom - rcMainWindow.top, SWP_NOZORDER);

	// Load the menus...
	hMenu = GetMenu(hWndProgMgr);
	hSystemMenu = GetSystemMenu(hWndProgMgr, FALSE);

	// Update relevant parts of the window
	UpdateChecks(bAutoArrange, IDM_OPTIONS, IDM_OPTIONS_AUTOARRANGE);
	UpdateChecks(bMinOnRun, IDM_OPTIONS, IDM_OPTIONS_MINONRUN);
	UpdateChecks(bTopMost, IDM_OPTIONS, IDM_OPTIONS_TOPMOST);
	UpdateChecks(bShowUsername, IDM_OPTIONS, IDM_OPTIONS_SHOWUSERNAME);
	UpdateChecks(bSaveSettings, IDM_OPTIONS, IDM_OPTIONS_SAVESETTINGS);

	UpdateWindowTitle();

	// Update settings based on their values
	if (bTopMost)
		SetWindowPos(hWndProgMgr, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (bIsDefaultShell)
	{
		// Modify the context menus since we're the default shell
		DeleteMenu(hSystemMenu, SC_CLOSE, MF_BYCOMMAND);

		LoadString(hAppInstance, IDS_SHUTDOWN, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_SHUTDOWN, szBuffer);
		ModifyMenu(hMenu, IDM_FILE_EXIT, MF_BYCOMMAND | MF_STRING, IDM_SHUTDOWN, szBuffer);

		LoadString(hAppInstance, IDS_RUN, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_FILE_RUN, szBuffer);

		LoadString(hAppInstance, IDS_TASKMGR, szBuffer, ARRAYSIZE(szBuffer));
		InsertMenu(hSystemMenu, 6, MF_BYPOSITION | MF_STRING, IDM_TASKMGR, szBuffer);
		
		// Create the desktop window...
		// CreateDesktopWindow();
	}

	// Create the frame window
	if (!InitializeGroups())
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if (!TranslateMDISysAccel(hWndMDIClient, &msg) &&
			!TranslateAccelerator(hWndProgMgr, hAccel, &msg))
		{
			DispatchMessage(&msg);
		}
	}

	return 0;
}


/*

// NOTE(u130b8): We used to compile Release builds with /NODEFAULTLIB
// but I've temporarily removed it so malloc and free works.

// NOTE(u130b8): We're compiling without the C runtime by default in Release builds.
// But in Debug builds, we need the C runtime, otherwise the address sanitizer and
// MSVC debug tools break because they use the wWinMainCRTStartup entrypoint to initialize.

#ifdef PROGMGR_RELEASE
#pragma function(memset)
void *memset(char* dst, int value, size_t count) {
    while (count--) { *dst++ = value; }
    return dst;
}

#pragma function(memcpy)
void *memcpy(char *dst, const char *src, size_t count) {
    while (count--) { *dst++ = *src++; }
    return dst;
}

void __stdcall wWinMainCRTStartup() {
    int code = wWinMain(GetModuleHandle(0), 0, 0, 0);
    ExitProcess(code);
}
#endif

*/