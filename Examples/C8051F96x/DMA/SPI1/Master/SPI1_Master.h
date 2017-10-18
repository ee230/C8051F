#ifndef SPI1_MASTER_H
#define SPI1_MASTER_H
//=============================================================================
// SPI1_Master.h
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
//=============================================================================
// Function Prototypes (API)
//=============================================================================
void SPI1_MasterOutIn (U16 length,
   VARIABLE_SEGMENT_POINTER (outPointer, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER (inPointer, U8, SEG_XDATA));

void SPI1_MasterOutOnly (U16 size,
   VARIABLE_SEGMENT_POINTER (outPointer, U8, SEG_XDATA));

//=============================================================================
#endif  // #ifdef SPI1_MASTER_H
//=============================================================================
// End of file
//=============================================================================
