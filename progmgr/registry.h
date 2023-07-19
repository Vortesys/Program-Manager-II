/* * * * * * * *\
	REGISTRY.H -
		Copyright (c) 2023 freedom7341, Freedom Desktop
	DESCRIPTION -
		Program Manager's header file for registry related functions.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Includes */
#include <wtypes.h>

/* Defines */
#define SHELL_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"
#define PROGMAN_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Program Manager"
#define WINDOWS_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
#define PROGMGR_KEY  L"Software\\Freedom Desktop\\Program Manager II"
#define REGKEYMAXLENGTH 261

/* Global Registry Keys */
//HKEY	hkeyProgramManager;
//HKEY	hkeyPMSettings;

/* Function Prototypes */
BOOL InitializeRegistryKeys(VOID);
BOOL IsProgMgrDefaultShell(VOID);
