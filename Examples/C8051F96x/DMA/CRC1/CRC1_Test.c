//=============================================================================
// CRC1_Test.c
//=============================================================================
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
//-----------------------------------------------------------------------------
// uncomment pragma SRC to generate assemble code
//-----------------------------------------------------------------------------
//#pragma SRC
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "C8051F960_defs.h"
#include "CRC1.h"
#include "CRC1_TestVectors.h"
//=============================================================================
// File global variables
//
// myData located in XRAM.
//
//=============================================================================
SEGMENT_VARIABLE (myData[32], U8, SEG_XDATA);
//=============================================================================
// Function Prototypes
//=============================================================================
//-----------------------------------------------------------------------------
// Interrupt proto (for SDCC compatibility)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO(DMA_ISR, INTERRUPT_DMA0);
//-----------------------------------------------------------------------------
// Functions used to initialize arrays.
//-----------------------------------------------------------------------------
void initCRCData (VARIABLE_SEGMENT_POINTER(xData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(cData, U8, SEG_CODE), U8 bytes);

//=============================================================================
// main
//=============================================================================
void main (void)
{
   volatile U16 results;
	U8 status;

   PCA0MD  &= ~0x40;                   // disable watchdog timer

   EA = 1;                             // enable global interrupts

   initCRCData(myData, BlockOne, 10);

   status = ComputeCRC1(10, 0x3d65, myData, &results);

	while(status);

   results = ~results;

   while(results!=0x4447);

   initCRCData(myData, BlockTwo, 6);

   status = ComputeCRC1(6, 0x3d65, myData, &results);

	while(status);

   results = ~results;

   while(results!=0x1E6D);

   while(1);
}
//-----------------------------------------------------------------------------
// DMA_ISR
// description:
//
// This ISR is needed to support the DMA Idle mode wake up, which is used
// in the AES functions. Bit 5 of EIE2 should be enabled before going into
// idle mode. This ISR will disable further interrupts. EA must also be
// enabled.
//
//-----------------------------------------------------------------------------
INTERRUPT(DMA_ISR, INTERRUPT_DMA0)
{
  EIE2 &= ~0x20;                       // disable further interrupts
}
//-----------------------------------------------------------------------------
// initCRCData()
//
// parameters:
//   xData          - xdata pointer to data in xram
//   cData          - code pointer to data in flash
//   bytes          - the number of bytes to copy
//
// returns:
//
// description:
//
// Copies reference cData in Flash to xData in xdata RAM.
// The code constants must be copied to xdata for use by the DMA.
//
//-----------------------------------------------------------------------------
void initCRCData (VARIABLE_SEGMENT_POINTER(xData, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER(cData, U8, SEG_CODE),
   U8 bytes)
{
   while(bytes--)
   {
      *xData++ = *cData++;
   }
}
//=============================================================================
// End of file
//=============================================================================

