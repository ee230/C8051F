//-----------------------------------------------------------------------------
// F500_FlashPrimitives.h
//-----------------------------------------------------------------------------
// Copyright 2008 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F500/1/2/3/4/5/6/7/8/9/10/11
// Tool chain:     Keil C51 8.00
// Command Line:   None
//
// Release 1.1 / 02 MAR 2010 (GP)
//    -Tested with Raisonance
//    -Updated to meet new data sheet guidelines for writing/erasing
//
// Release 1.0
//    -Initial Revision (GP)
//    -08 APR 2008
//

#include <compiler_defs.h>

//-----------------------------------------------------------------------------
// Open Header #define
//-----------------------------------------------------------------------------

#ifndef _F500_FLASHPRIMITIVES_H_
#define _F500_FLASHPRIMITIVES_H_

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
#define FLASH_TEMP 0xF800L             // For 64K Flash devices
//#define FLASH_TEMP 0x7C00L           // For 32K Flash devices
#endif

#ifndef FLASH_LAST
#define FLASH_LAST 0xFA00L             // For 64K Flash devices
//#define FLASH_LAST 0x7E00L           // For 32K Flash devices
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

#endif    // _F500_FLASHPRIMITIVES_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------