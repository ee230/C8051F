//-----------------------------------------------------------------------------
// F540_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F54x
// Tool chain:     Keil C51 8.00
// Command Line:   None
//
//
// Release 1.1 / 10 MAR 2010 (GP)
//    -Tested with Raisonance
//    -Updated to meet new data sheet guidelines for writing/erasing
//
// Release 1.0 / 05 NOV 2008 (GP)
//    -Initial Revision
//

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Open Header #define
//-----------------------------------------------------------------------------

#ifndef _F540_FLASHPRIMITIVES_H_
#define _F540_FLASHPRIMITIVES_H_

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

typedef U16 FLADDR;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#ifndef FLASH_PAGESIZE
#define FLASH_PAGESIZE 512
#endif

#ifndef FLASH_TEMP
#define FLASH_TEMP 0x3800L             // For 16K Flash devices
//#define FLASH_TEMP 0x1C00L           // For  8K Flash devices
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0x3A00L             // For 16K Flash devices
//#define FLASH_LAST 0x1E00L           // For  8K Flash devices
#endif

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

U8   FLASH_ByteWrite (FLADDR addr, U8 byte);
U8   FLASH_ByteRead  (FLADDR addr);
U8   FLASH_PageErase (FLADDR addr);

//-----------------------------------------------------------------------------
// Close Header #define
//-----------------------------------------------------------------------------

#endif    // _F540_FLASHPRIMITIVES_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------