/* * * * * * * *\
	REGISTRY.C -
		Copyright (c) 2023 freedom7341, Vortesys
	DESCRIPTION -
		Program Manager's registry and settings related functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <strsafe.h>

/* Variables */
// Global HKEYs
HKEY hKeyProgramManager = NULL;
HKEY hKeyProgramGroups = NULL;
HKEY hKeySettings = NULL;
// Global Values
BOOL bSaveSettings;
BOOL bShowUsername;
BOOL bTopMost;
BOOL bMinOnRun;
BOOL bAutoArrange;
DWORD dwSettingsMask = {
	PMS_SAVESETTINGS |
	PMS_SHOWUSERNAME |
	!PMS_TOPMOST |
	!PMS_MINONRUN |
	PMS_AUTOARRANGE
};
RECT rcMainWindow;
// Registry Subkeys
PWSTR pszProgramGroups = L"Program Groups";
PWSTR pszSettings = L"Settings";
// Settings Subkeys
PWSTR pszSettingsWindow = L"Window";
PWSTR pszSettingsMask = L"SettingsMask";

/* Functions */

/* * * *\
	InitializeRegistryKeys -
		Takes the relevant registry keys and turns them
		into valid and usable handles.
	RETURNS -
		TRUE if successful, FALSE if unsuccessful.
\* * * */
BOOL InitializeRegistryKeys()
{
	if (!RegCreateKeyEx(HKEY_CURRENT_USER, PROGMGR_KEY, 0, szProgMgr, 0,
		KEY_READ | KEY_WRITE, NULL, &hKeyProgramManager, NULL))
	{
		// Create Program Groups and Settings keys
		RegCreateKeyEx(hKeyProgramManager, pszProgramGroups, 0, szProgMgr, 0,
			KEY_READ | KEY_WRITE, NULL, &hKeyProgramGroups, NULL);
		RegCreateKeyEx(hKeyProgramManager, pszSettings, 0, szProgMgr, 0,
			KEY_READ | KEY_WRITE, NULL, &hKeySettings, NULL);
		
		return TRUE;
	}
	return FALSE;
}

/* * * *\
	IsProgMgrDefaultShell -
		Detects if Program Manager is the default shell.
	RETURNS -
		TRUE if Program Manager is the default shell,
		FALSE if otherwise or an error occurs.
\* * * */
BOOL IsProgMgrDefaultShell()
{
	HKEY hKeyWinlogon;
	WCHAR szShell[HKEYMAXLEN] = L"";
	DWORD dwType;
	DWORD dwBufferSize = sizeof(szShell);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINLOGON_KEY,
		0, KEY_READ, &hKeyWinlogon) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKeyWinlogon, L"Shell", 0, &dwType,
			(LPBYTE)szShell, &dwBufferSize) == ERROR_SUCCESS)
		{
			if (StrStr(szShell, szProgMgr))
			{
				// ProgMgr detected >:)
				RegCloseKey(hKeyWinlogon);
				return TRUE;
			}
			else
			{
				// Inferior shell detected.
				RegCloseKey(hKeyWinlogon);
				return FALSE;
			}
		}
	}
	else
	{
		// Assume that we're the shell... just incase.
		RegCloseKey(hKeyWinlogon);
		return TRUE;
	}
	// No registry access.
	return FALSE;
}

/* * * *\
	SaveConfig -
		Finds and collapses all settings
		and saves them to the registry.
	RETURNS -
		TRUE if successful, FALSE if unsuccessful.
\* * * */
BOOL SaveConfig(BOOL bPos, BOOL bSettings, BOOL bGroups)
{
	BOOL bConfigStatus = TRUE;
	WINDOWPLACEMENT wpProgMgr;
	RECT rcWindow;

	if (bSettings)
	{
		// Shrink the settings into the bitmask and save it
		// There's definitely a better way to do this but...
		// I'll get to it later :)
		dwSettingsMask =
			(bAutoArrange && PMS_AUTOARRANGE) * PMS_AUTOARRANGE |
			(bMinOnRun && PMS_MINONRUN) * PMS_MINONRUN |
			(bTopMost && PMS_TOPMOST) * PMS_TOPMOST |
			(bShowUsername && PMS_SHOWUSERNAME) * PMS_SHOWUSERNAME |
			(bSaveSettings && PMS_SAVESETTINGS) * PMS_SAVESETTINGS;

		if (!RegSetValueEx(hKeySettings, pszSettingsMask, 0, REG_DWORD,
			(const BYTE*)&dwSettingsMask, sizeof(dwSettingsMask)) == ERROR_SUCCESS)
			bConfigStatus = FALSE;
	}

	if (bPos)
	{
		// Get and save window position
		wpProgMgr.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hWndProgMgr, &wpProgMgr);
		CopyRect(&rcWindow, &wpProgMgr.rcNormalPosition);

		if (!RegSetValueEx(hKeySettings, pszSettingsWindow, 0, REG_BINARY,
			(const BYTE*)&rcWindow, sizeof(rcWindow)) == ERROR_SUCCESS)
			bConfigStatus = FALSE;
	}

	return bConfigStatus;
}

/* * * *\
	BOOL LoadConfig() -
		Finds all settings and retrieves them
		from the registry.
	RETURNS -
		TRUE if successful, FALSE if unsuccessful.
\* * * */
BOOL LoadConfig()
{
	BOOL bConfigStatus = TRUE;
	DWORD dwType;
	DWORD dwBufferSize = sizeof(dwSettingsMask);
	DWORD dwRectBufferSize = sizeof(rcMainWindow);

	// Load settings bitmask
	if (!RegQueryValueEx(hKeySettings, pszSettingsMask, 0, &dwType,
		(LPBYTE)&dwSettingsMask, &dwBufferSize) == ERROR_SUCCESS)
		bConfigStatus = FALSE;
	
	// Apply bitmask to booleans
	bAutoArrange = (dwSettingsMask & PMS_AUTOARRANGE);
	bMinOnRun = (dwSettingsMask & PMS_MINONRUN);
	bTopMost = (dwSettingsMask & PMS_TOPMOST);
	bSaveSettings = (dwSettingsMask & PMS_SAVESETTINGS);
	bShowUsername = (dwSettingsMask & PMS_SHOWUSERNAME);

	// Load window position... and apply it!
	if (!RegQueryValueEx(hKeySettings, pszSettingsWindow, 0, &dwType,
		(LPBYTE)&rcMainWindow, &dwRectBufferSize) == ERROR_SUCCESS)
		bConfigStatus = FALSE;

	return bConfigStatus;
}
