/* * * * * * * *\
	REGISTRY.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's registry related functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "progmgr.h"
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>

/* Functions */
/* * * *\
	InitializeRegistryKeys -
		Takes the relevant Registry Keys and turns them
		into valid and usable handles.
	RETURNS -
		True if successful, false if unsuccessful.
\* * * */
BOOL InitializeRegistryKeys()
{
	HKEY	hkeyProgramManager;
	//HKEY	hkeyPMSettings;

	if (!RegCreateKeyEx(HKEY_CURRENT_USER, PROGMGR_KEY, 0, 0, 0, KEY_READ | KEY_WRITE, NULL, &hkeyProgramManager, NULL)) {
		return TRUE;
	}
	return FALSE;
}

/* * * *\
	IsProgMgrDefaultShell -
		Detects if Program Manager is the default shell.
	RETURNS -
		True if Program Manager is the default shell,
		false if otherwise or an error occurs.
\* * * */
BOOL IsProgMgrDefaultShell()
{
	HKEY hKeyWinlogon;
	WCHAR szShell[HKEYMAXLEN] = L"";
	DWORD dwType;
	DWORD dwBufferSize;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, SHELL_KEY, 0, KEY_READ, &hKeyWinlogon) == ERROR_SUCCESS) {
		if (RegQueryValueEx(hKeyWinlogon, L"Shell", 0, &dwType, (LPBYTE)szShell, &dwBufferSize) == ERROR_SUCCESS) {
			if (StrStr(szShell, szProgMgr)) {
				// ProgMgr detected >:)
				RegCloseKey(hKeyWinlogon);
				return TRUE;
			}
			else {
				// Inferior shell detected.
				RegCloseKey(hKeyWinlogon);
				return FALSE;
			}
		}
	}
	else {
		// Assume that we're the shell... just incase.
		RegCloseKey(hKeyWinlogon);
		return TRUE;
	}
	// No registry access.
	return FALSE;
}