#ifndef CRC1_H
#define CRC1_H
//=============================================================================
// CRC1.h
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// Target:
//    C8051F960
//
// IDE:
//    Silicon Laboratories IDE
//
// Tool Chains:
//    Keil
//    SDCC
//    Raisonance
//
// Project Name:
//    C8051F960 Code Examples
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
#ifndef COMPILER_DEFS_H
#include <compiler_defs.h>
#endif
//-----------------------------------------------------------------------------
// Define used with CRC1CN
//-----------------------------------------------------------------------------
#define CRC1_CLEAR      0x80
#define CRC1_DMA_ENABLE 0x08
#define CRC1_FLIP       0x04
#define CRC1_INV        0x02
#define CRC1_SEED_POL   0x01
//-----------------------------------------------------------------------------
// Define SUCCESS if not already defined
//-----------------------------------------------------------------------------
#ifndef SUCCESS
#define SUCCESS 0
#elif(SUCCESS!=0)
#error  "SUCCESS definition conflict!"
#endif
//-----------------------------------------------------------------------------
// typedef for status
//-----------------------------------------------------------------------------
typedef U8 CRC1_STATUS;
//-----------------------------------------------------------------------------
// Enumerated type for error codes.
//-----------------------------------------------------------------------------
enum CRC1_ERROR_CODES_Enum
{
   CRC1_ERROR_INVALID_POLYNOMIAL = 1,  // 0x01
   CRC1_ERROR_ZERO_LENGTH              // 0x02
};
//=============================================================================
// Function Prototypes (API)
//=============================================================================
CRC1_STATUS ComputeCRC1 (U16 size, U16 polynomial,
   VARIABLE_SEGMENT_POINTER(input, U8, SEG_XDATA), U16 *results);
//=============================================================================
// End of file
//=============================================================================
#endif  // #ifdef CRC1_H
//=============================================================================
