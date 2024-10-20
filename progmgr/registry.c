/* * * * * * * *\
	REGISTRY.C -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Program Manager's registry and settings related functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#include <strsafe.h>
#include "progmgr.h"
#include "group.h"
#include "registry.h"

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
PWSTR pszProgramGroups = TEXT("Program Groups");
PWSTR pszSettings = TEXT("Settings");
// Settings Subkeys
PWSTR pszSettingsWindow = TEXT("Window");
PWSTR pszSettingsMask = TEXT("SettingsMask");
// Permissions (Global)

/* Functions */

/* * * *\
	InitializeRegistryKeys -
		Takes the relevant registry keys and turns them
		into valid and usable handles.
	RETURNS -
		TRUE if successful, FALSE if unsuccessful.
\* * * */
BOOL InitializeRegistryKeys(VOID)
{
	if (!RegCreateKeyEx(HKEY_CURRENT_USER, PROGMGR_KEY, 0, g_szProgMgr, 0,
		KEY_READ | KEY_WRITE, NULL, &hKeyProgramManager, NULL))
	{
		// Create Program Groups and Settings keys
		RegCreateKeyEx(hKeyProgramManager, pszProgramGroups, 0, g_szProgMgr, 0,
			KEY_READ | KEY_WRITE, NULL, &hKeyProgramGroups, NULL);
		RegCreateKeyEx(hKeyProgramManager, pszSettings, 0, g_szProgMgr, 0,
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
BOOL IsProgMgrDefaultShell(VOID)
{
	HKEY hKeyWinlogon;
	WCHAR szShell[HKEYMAXLEN] = TEXT("");
	DWORD dwType;
	DWORD dwBufferSize = sizeof(szShell);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, WINLOGON_KEY,
		0, KEY_READ, &hKeyWinlogon) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKeyWinlogon, TEXT("Shell"), 0, &dwType,
			(LPBYTE)szShell, &dwBufferSize) == ERROR_SUCCESS)
		{
			if (StrStr(szShell, g_szProgMgr))
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
	RegistrySaveGroup -
		Saves a group structure to registry.
	RETURNS -
		RCE_* configuration error value
\* * * */
DWORD RegistrySaveGroup(_In_ PGROUP pg)
{
	DWORD dwConfigStatus = RCE_SUCCESS;

	// If the pointer is invalid then fail out
	if (pg == NULL)
		return RCE_FAILURE;

	// Save group
	UpdateGroup(pg);
	if (!RegSetValueEx(hKeyProgramGroups, pg->szName, 0, REG_BINARY,
		(const BYTE*)pg, sizeof(*pg)) == ERROR_SUCCESS)
		dwConfigStatus = dwConfigStatus && RCE_GROUPS;

	return dwConfigStatus;
}

/* * * *\
	SaveConfig -
		Finds and collapses all settings
		and saves them to the registry.
	RETURNS -
		RCE_* configuration error value
\* * * */
DWORD SaveConfig(_In_ BOOL bSettings, _In_ BOOL bPos, _In_ BOOL bGroups, _In_ BOOL bExit)
{
	DWORD dwConfigStatus = RCE_SUCCESS;

	if (bSettings)
	{
		// Shrink the settings into the bitmask and save it
		// There's definitely a better way to do this but...
		// TODO: do this more efficiently
		dwSettingsMask =
			(bAutoArrange && PMS_AUTOARRANGE) * PMS_AUTOARRANGE |
			(bMinOnRun && PMS_MINONRUN) * PMS_MINONRUN |
			(bTopMost && PMS_TOPMOST) * PMS_TOPMOST |
			(bShowUsername && PMS_SHOWUSERNAME) * PMS_SHOWUSERNAME |
			(bSaveSettings && PMS_SAVESETTINGS) * PMS_SAVESETTINGS;

		if (!RegSetValueEx(hKeySettings, pszSettingsMask, 0, REG_DWORD,
			(const BYTE*)&dwSettingsMask, sizeof(dwSettingsMask)) == ERROR_SUCCESS)
			dwConfigStatus = dwConfigStatus && RCE_SETTINGS;
	}

	if (bPos)
	{
		WINDOWPLACEMENT wpProgMgr = { 0 };
		RECT rcWindow;

		// Get and save window position
		wpProgMgr.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(g_hWndProgMgr, &wpProgMgr);
		CopyRect(&rcWindow, &wpProgMgr.rcNormalPosition);

		if (!RegSetValueEx(hKeySettings, pszSettingsWindow, 0, REG_BINARY,
			(const BYTE*)&rcWindow, sizeof(rcWindow)) == ERROR_SUCCESS)
			dwConfigStatus = dwConfigStatus && RCE_POSITION;
	}

	if (bGroups)
	{
		HWND hWndGroup = NULL;

		EnumChildWindows(hWndMDIClient, &SaveWindowEnumProc, (LPARAM)bExit);
	}

	return dwConfigStatus;
}

/* * * *\
	SaveWindowEnumProc() -
		Procedure for enumeration
		of group windows to save
		or close them
	NOTES - 
		lParam is a BOOL that determines
		whether to close the group or not
	RETURNS -
		TRUE to continue
		FALSE to stop
\* * * */
BOOL CALLBACK SaveWindowEnumProc(HWND hWndGroup, LPARAM lParam)
{
	PGROUP pNewGroup = NULL;
	PGROUP pGroup = NULL;

	if (hWndGroup == NULL)
		return FALSE;

	pGroup = (PGROUP)GetWindowLongPtr(hWndGroup, GWLP_USERDATA);

	if (pGroup == NULL)
		return FALSE;

	// lean it...
	/*pNewGroup = realloc(pGroup, CalculateGroupMemory(pGroup, 1, 1));
	if (pNewGroup != NULL)
	{
		pGroup = pNewGroup;
		SetWindowLongPtr(hWndGroup, GWLP_USERDATA, (LONG_PTR)pGroup);
	}*/

	// save it...
	if (RegistrySaveGroup(pGroup) != RCE_SUCCESS)
		return FALSE;

	// close it...
	if ((BOOL)lParam == TRUE)
	{
		RemoveGroup(hWndGroup, FALSE);
		return FALSE;
	}

	return TRUE;
}

/* * * *\
	LoadConfig() -
		Finds all settings and retrieves them
		from the registry.
	RETURNS -
		RCE_* configuration error value
\* * * */
DWORD LoadConfig(_In_ BOOL bSettings, _In_ BOOL bPos, _In_ BOOL bGroups)
{
	DWORD dwConfigStatus = RCE_SUCCESS;

	if (bSettings)
	{
		DWORD dwBufferSize = sizeof(dwSettingsMask);
		DWORD dwType = REG_DWORD;

		// Load settings bitmask
		if (!RegQueryValueEx(hKeySettings, pszSettingsMask, 0, &dwType,
			(LPBYTE)&dwSettingsMask, &dwBufferSize) == ERROR_SUCCESS)
			dwConfigStatus = dwConfigStatus && RCE_SETTINGS;

		// Apply bitmask to booleans
		bAutoArrange = (dwSettingsMask & PMS_AUTOARRANGE);
		bMinOnRun = (dwSettingsMask & PMS_MINONRUN);
		bTopMost = (dwSettingsMask & PMS_TOPMOST);
		bSaveSettings = (dwSettingsMask & PMS_SAVESETTINGS);
		bShowUsername = (dwSettingsMask & PMS_SHOWUSERNAME);
	}

	if (bPos)
	{
		DWORD dwRectBufferSize = sizeof(rcMainWindow);
		DWORD dwType = REG_BINARY;

		// Load window position
		if (!RegQueryValueEx(hKeySettings, pszSettingsWindow, 0, &dwType,
			(LPBYTE)&rcMainWindow, &dwRectBufferSize) == ERROR_SUCCESS)
			dwConfigStatus = dwConfigStatus && RCE_POSITION;
	}

	if (bGroups)
	{
		DWORD dwBufferSize = sizeof(dwSettingsMask);
		DWORD dwType = REG_BINARY;
		UINT cGroupVals = 0;

		OutputDebugString(TEXT("REGISTRY.C: Loading groups...\n"));

		// Get the num of group values, if we fail then ABORT!
		if (!(RegQueryInfoKey(hKeyProgramGroups, NULL, NULL, NULL,
			NULL, NULL, NULL, &cGroupVals,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS))
		{
			dwConfigStatus = dwConfigStatus && RCE_SETTINGS;

			// I can get away with returning here upon
			// failure since this is the last one
			OutputDebugString(TEXT("REGISTRY.C: Failed to get # of values!\n"));
			return dwConfigStatus;
		}

		// Enumerate through the registry values
		if (cGroupVals)
		{
			DWORD i = 0;

			for (i = 0; i < cGroupVals; i++)
			{
				WCHAR szGroupValName[MAX_TITLE_LENGTH] = TEXT("");
				UINT cbGroupValName = MAX_TITLE_LENGTH;
				UINT cbGroup = 0;
				PGROUP pGroup = NULL;
				LSTATUS error = ERROR_SUCCESS;

				// TODO: figure out where i'm really going to store the
				// group name, if not in the group structure then in
				// the name of the registry key (val 3 here)

				// get the size of the group
				error = RegEnumValue(hKeyProgramGroups, i, (LPWSTR)&szGroupValName,
					&cbGroupValName, NULL, NULL, NULL, &cbGroup);

				if (error == ERROR_NO_MORE_ITEMS)
					break;
				else if (error != ERROR_SUCCESS)
				{
					OutputDebugString(TEXT("REGISTRY.C: RegEnumValue failure.\n"));
					break;
				}


				// allocate memory for the group
				pGroup = malloc(cbGroup);

				if (pGroup == NULL)
				{
					dwConfigStatus = dwConfigStatus && RCE_GROUPS;
					return dwConfigStatus;
				}

				// retrieve the group
				// TODO: add error_checking, ERROR_FILE_NOT_FOUND
				// stuff like that, please
				// error checking makes reliability....
				error = RegGetValue(hKeyProgramGroups, NULL, szGroupValName, RRF_RT_REG_BINARY, NULL, pGroup, &cbGroup);
				/*error = RegQueryValueEx(hKeyProgramGroups, szGroupValName, NULL, NULL, pGroup, &cbGroup);*/
				if (error == ERROR_FILE_NOT_FOUND)
				{
					OutputDebugString(TEXT("REGISTRY.C: Group not found!\n"));

					dwConfigStatus = dwConfigStatus && RCE_GROUPS;
				}
				if (error == ERROR_MORE_DATA)
				{
					OutputDebugString(TEXT("REGISTRY.C: Buffer too small!\n"));

					dwConfigStatus = dwConfigStatus && RCE_GROUPS;
				}
				else if (error != ERROR_SUCCESS)
				{
					OutputDebugString(TEXT("REGISTRY.C: Group retrieval failed!\n"));

					dwConfigStatus = dwConfigStatus && RCE_GROUPS;
				}
				else
					OutputDebugString(TEXT("REGISTRY.C: Group retrieved.\n"));

				// create the group
				if (pGroup)
				{
					if (pGroup->dwSignature == GRP_SIGNATURE)
					{
						CreateGroup(pGroup);
						OutputDebugString(TEXT("REGISTRY.C: Group created.\n"));
					}
					else
					{
						OutputDebugString(TEXT("REGISTRY.C: Group creation aborted.\n"));
					}

					// free memory
					free(pGroup);
				}
				else
				{
					dwConfigStatus = dwConfigStatus && RCE_GROUPS;
					OutputDebugString(TEXT("REGISTRY.C: Failed to create group!\n"));
				}
			}
		}
	}

	return dwConfigStatus;
}
