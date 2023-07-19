/* * * * * * * *\
	REGISTRY.C -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's registry related functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Headers */
#include "registry.h"
// #define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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