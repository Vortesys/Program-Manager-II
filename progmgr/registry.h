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
#define HKEYMAXLEN 261
// Key Paths
#define WINLOGON_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"
// #define PROGMAN_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Program Manager"
// #define WINDOWS_KEY  L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows"
#define PROGMGR_KEY  L"Software\\Freedom Desktop\\Program Manager II"
// Settings Bitmask Values (DWORD)
#define PMS_AUTOARRANGE 0x00000001
#define PMS_MINONRUN 0x00000002
#define PMS_TOPMOST 0x00000008
#define PMS_SHOWUSERNAME 0x00000004
#define PMS_SAVESETTINGS 0x00000010

/* Global Variables */
// HKEYs
extern HKEY	hKeyProgramManager;
extern HKEY	hkeyProgramGroups;
extern HKEY	hKeySettings;
// Settings
extern BOOL bAutoArrange;
extern BOOL bMinOnRun;
extern BOOL bTopMost;
extern BOOL bShowUsername;
extern BOOL bSaveSettings;

/* Function Prototypes */
BOOL InitializeRegistryKeys(VOID);
BOOL IsProgMgrDefaultShell(VOID);
BOOL LoadConfig(VOID);
BOOL SaveConfig(VOID);
