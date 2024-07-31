/* * * * * * * *\
	RESOURCE.H -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Program Manager's resource collection.
		** NO DEPENDENCIES **
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#ifndef RC_INVOKED
#pragma once
#endif

/* Icon Library */
// Primary, 1-9
#define IDI_PROGMGR		1
#define IDI_PROGGRP		2
#define IDI_PROGITM		3
#define IDI_COMMON		4
#define IDI_PERSONAL	5
// Secondary, 10-19
// #define IDI_COMMS		10
// #define IDI_CONSOLE		11
// #define IDI_DOCUMENT	12
// #define IDI_SPREADSHEET	13
// Tertiary, 20+
// #define IDI_SUPERMAN	20

/* Menus and Commands */
// Context Menus and Accelerators
#define IDM_MAIN					100
#define IDM_ITEM					101
#define IDM_GROUP					102
#define IDA_ACCELS					103
// File, 110-119
#define IDM_FILE					110
#define IDM_FILE_NEW				111
#define IDM_FILE_NEW_GROUP			112
#define IDM_FILE_NEW_ITEM			113
#define IDM_FILE_OPEN				114
#define IDM_FILE_MOVE				115
#define IDM_FILE_COPY				116
#define IDM_FILE_DELETE				117
#define IDM_FILE_PROPS				118
#define IDM_FILE_RUN				119
#define IDM_FILE_EXIT				120
// Options, 130-149
#define IDM_OPTIONS					130
#define IDM_OPTIONS_AUTOARRANGE		131
#define IDM_OPTIONS_MINONRUN		132
#define IDM_OPTIONS_TOPMOST			133
#define IDM_OPTIONS_SHOWUSERNAME	134
#define IDM_OPTIONS_SAVESETTINGS	135
#define IDM_OPTIONS_SAVENOW			136
// Window, 150-169, 5000+
#define IDM_WINDOW					150
#define IDM_WINDOW_CASCADE			151
#define IDM_WINDOW_TILE				152
#define IDM_WINDOW_TILEHORIZONTALLY	153
#define IDM_WINDOW_ARRANGEICONS		154
#define IDM_WINDOW_CHILDSTART		5000
// Help, 170-179
#define IDM_HELP					170
#define IDM_HELP_INDEX				171
#define IDM_HELP_ABOUT				172
// Miscellaneous, 180-199
#define IDM_SHUTDOWN				180
#define IDM_TASKMGR					181
// Desktop Menus, 200-299
#define IDM_DESK					200
#define IDM_DESK_VIEW				201
#define IDM_DESK_SORT				202
#define IDM_DESK_SCREENRES			203
#define IDM_DESK_PERSONALIZE		204

/* Dialogs */
#define DLG_GROUP	1
#define DLG_ITEM	2
#define DLG_POWER	3

/* Dialog Controls */
// Buttons
#define IDD_OK				1
#define IDD_CANCEL			2
#define IDD_BROWSE			3
#define IDD_BROWSE2			4
#define IDD_CHICON			5
// Radio Buttons
#define IDD_WORKDIR			100
#define IDD_COMMGROUP		101
// Input Controls
#define IDD_NAME			200
#define IDD_PATH			201
#define IDD_WORKPATH		202
#define IDD_HOTKEY			203
// Static Controls
#define IDD_STAT_ICON		300
#define IDD_STAT_WORKDIR	301

/* String Table */
#define IDS_APPTITLE		1
#define IDS_PMCLASS			2
#define IDS_DTCLASS			3
#define IDS_GRPCLASS		4
#define IDS_WEBSITE			5
// Menu Commands
#define IDS_RUN				100
#define IDS_TASKMGR			101
#define IDS_EXIT			102
#define IDS_SHUTDOWN		103
// General Dialog
#define IDS_DLG_OK			200
#define IDS_DLG_CANCEL		201
#define IDS_DLG_BROWSE		202
// Icon Section
#define IDS_DLG_ICON		203
#define IDS_DLG_CHICON		204
// Properties Section
#define IDS_DLG_PROPS		205
#define IDS_DLG_NAME		206
#define IDS_DLG_DIRECTORY	207
#define IDS_DLG_COMMGRP		208
// Advanced Section
#define IDS_DLG_ADVANCED	209
#define IDS_DLG_WORKDIR		210
#define IDS_DLG_HOTKEY		211
// Dialog Title
#define IDS_DLT_GRP_PROPS	300
#define IDS_DLT_GRP_NEW		301
#define IDS_DLT_ITEM_PROPS	302
#define IDS_DLT_ITEM_NEW	303
#define IDS_DLT_POWER		304
