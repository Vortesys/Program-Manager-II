/* * * * * * * *\
	GRPUPDAT.H -
		Copyright (c) 2024 Vortesys, Brady McDermott
	DESCRIPTION -
		Header containing classic program group structures
		in order to facilitate conversion to the new format.
	LICENSE INFORMATION -
		MIT License, see LICENSE.txt in the root folder
\* * * * * * * */

/* Pragmas */
#pragma once

/* Includes */
#include <wtypes.h>

/* Definitions */
// Group signatures
#define OLDGRP_SIGNATURE 0x43554D50L // PMUC

/* Structures */
// Old group format, .GRP
typedef struct _OLDGROUP {
	DWORD dwMagic; // Set to OLDGRP_SIGNATURE
	WORD wCheckSum; // Seemingly unused
    WORD cbGroup; // Size of group
    RECT rcNormal; // Window rect
    POINT ptMin; // Minimized window position
    WORD nCmdShow; // Min/max state
    WORD pName; // Group name
    WORD cxIcon; // Icon width
    WORD cyIcon; // Icon height
    WORD wIconFormat;
    WORD wReserved; // Unused

    WORD cItems; // Number of items
    WORD rgiItems[1]; // Array of item offsets
} OLDGROUP, * POLDGROUP;
