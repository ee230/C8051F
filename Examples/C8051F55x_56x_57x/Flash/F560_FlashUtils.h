//-----------------------------------------------------------------------------
// F560_FlashUtils.h
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// FLASH memory.
//
// Target:         C8051F560
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    -Initial Revision
//

//-----------------------------------------------------------------------------
// Open Header #define
//-----------------------------------------------------------------------------

#ifndef _F560_FLASHUTILS_H_
#define _F560_FLASHUTILS_H_

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "F560_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
void FLASH_Write (FLADDR dest, U8 *src, U16 numbytes);
U8 * FLASH_Read (U8 *dest, FLADDR src, U16 numbytes);
void FLASH_Clear (FLADDR addr, U16 numbytes);

// FLASH update/copy routines
void FLASH_Update (FLADDR dest, U8 *src, U16 numbytes);
void FLASH_Copy (FLADDR dest, FLADDR src, U16 numbytes);

// FLASH test routines
void FLASH_Fill (FLADDR addr, U16 length, U8 fill);

//-----------------------------------------------------------------------------
// Close Header #define
//-----------------------------------------------------------------------------

#endif    // _F560_FLASHUTILS_H_

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------