//-----------------------------------------------------------------------------
// F580_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2004 Silicon Laboratories, Inc.
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F580
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//    -Updated to meet new data sheet guidelines for writing/erasing
//
// Release 1.0 / 23 OCT 2008 (GP)
//    -Initial Revision


#ifndef F580_FLASHPRIMITIVES_H
#define F580_FLASHPRIMITIVES_H

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "compiler_defs.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define FLASH_SIZE  128                // 128K or 96K

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512L
#endif

#ifndef FLASH_SCRATCHSIZE
#define FLASH_SCRATCHSIZE 256
#endif

//---------------------

#if (FLASH_SIZE == 128)

#ifndef FLASH_TEMP                     // Address of temp page
#define FLASH_TEMP 0x1F800L            // For 128K devices
#endif

#ifndef FLASH_LAST                     // Last page of FLASH
#define FLASH_LAST 0x1FA00L            // For 128K devices
#endif

#endif

//---------------------

#if (FLASH_SIZE == 96)

#ifndef FLASH_TEMP                     // Address of temp page
#define FLASH_TEMP 0x17D00L            // For 128K devices
#endif

#ifndef FLASH_LAST                     // Last page of FLASH
#define FLASH_LAST 0x17B00L            // For 128K devices
#endif

#endif

//---------------------

typedef U32 FLADDR;                    // Allows for 17-bit address


//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines

U8   FLASH_ByteWrite (FLADDR addr, U8 byte, bit SFLE);
U8   FLASH_ByteRead  (FLADDR addr, bit SFLE);
U8   FLASH_PageErase (FLADDR addr, bit SFLE);

#endif // F580_FLASHPRIMITIVES_H


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------