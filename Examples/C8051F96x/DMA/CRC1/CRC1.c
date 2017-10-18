//=============================================================================
// CRC1.c
//=============================================================================
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//    This C file is a reusable code module for the CRC1 module.
//
//    Please see the API function description below for a detailed explanation
//    of the reusable functions.
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
// uncomment pragma SRC to generate assembly code
//-----------------------------------------------------------------------------
//#pragma SRC
//=============================================================================
// Includes
//=============================================================================
#include <compiler_defs.h>
#include <C8051F960_defs.h>
#include "DMA_defs.h"
#include "CRC1.h"
//=============================================================================
// API Functions
//=============================================================================
//-----------------------------------------------------------------------------
// ComputeCRC1()
//
// parameters:
//    size            - size for input data
//    polynomial      - 16-bit polynomial in normal msb first notation
//    input           - xdata pointer to unencoded data
//    results*        - generic pointer for 16-bit results
//
//
// returns:
//    status         - 0 for SUCCESS
//                   - 1 for CRC1_ERROR_INVALID_POLYNOMIAL
//                   - 2 for CRC_ERROR_ZERO_LENGTH
//
// Description:
//
// A single function is provided computing the CRC of data in xram
// using an arbitrary 16-bit polynomial.
//
// This function uses CRC1 peripheral with the DMA.
// The DMA channel allocations are defined in DMA_defs.h.
//
// This function puts the MCU code into idle mode while CRC computation
// is in process.
//
// A interrupt is needed to wake up from Idle mode. This requires that the
// global interrupts are enabled. Also, a small DMA ISR that disables EIE2
// bit 5 is required in the main module.
//
// This function will return an error for an invalid size or invalid
// polynomial. An error should never occur when using a valid polynomial
// and non-zero length.
//
// A 16-bit polynomial always has a x^16 term and a 1 (x^0) term.
// The x^16 term is not included in the normal 16-bit representation.
// So x^16 + 1 would be represented as 0x0001.
//
// The CRC1 module cannot be used with a lesser degree polynomial.
//
// Note that the results are stored in the location specified by the *results
// pointer parameter. This is pointer to a 16-bit unsigned int variable
// that is not specific to any memory type. The calling firmware must create
// a local, or global, U16 variable and pass a pointer to this location.
// When using a U16 variable, an ampersand (&) operator should be used to pass
// the address. A ampersand should not be used when passing an actual pointer.
// Since this is a generic memory pointer, the local variable may be stored in
// any memory space. Local variables declared without a memory spcifier may be
// stored according to the project memory model.
//
//-----------------------------------------------------------------------------
CRC1_STATUS ComputeCRC1 (U16 size, U16 polynomial,
   VARIABLE_SEGMENT_POINTER(input, U8, SEG_XDATA), U16 *results)
{
   // unions used for compiler independent endianness.
   UU16 value;

   // polynomial must have a x^0 term
   if ((polynomial&0x0001) == 0)
   {
      return CRC1_ERROR_INVALID_POLYNOMIAL;
   }

   // check for zero size
   if (size == 0)
   {
      return CRC1_ERROR_ZERO_LENGTH;
   }

   SFRPAGE = DPPE_PAGE;

   CRC1CN = 0x80;                      // clear CRC results to zero

   // Set CRC1 polynomial
   value.U16 = polynomial;
   CRC1POLL = value.U8[LSB];
   CRC1POLH = value.U8[MSB];

   DMA0EN &= ~CRC1_IN_MASK;             // disable CRC1 DMA

   // Configure DMA CRC1 IN channel
   // Select channel and peripheral request
   // Enable interrupt
   // Disable wrapping. (Only used for AES Key.)
   DMA0SEL = CRC1_IN_CHANNEL;
   DMA0NCF = CRC1_PERIPHERAL_REQUEST|DMA_INT_EN;
   DMA0NMD = NO_WRAPPING;

   // Initialize base address using input pointer.
   value.U16 = (U16)input;
   DMA0NBAL = value.U8[LSB];
   DMA0NBAH = value.U8[MSB];

   // Initialize size using size parameter.
   value.U16 = size;
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];

   // Clear address offset.
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // Clear CRC1 DMA bit in DMA0INT sfr using mask.
   DMA0INT &= ~CRC1_IN_MASK;
   // Set CRC1 DMA bit in DMA0EN sfr using mask.
   DMA0EN  |=  CRC1_IN_MASK;

   // Set CRC1 DMA mode enable bit to start operation
   CRC1CN |=  CRC1_DMA_ENABLE;

   // enable DMA interrupt to terminate Idle mode
   EIE2 |= 0x20;

   // This do...while loop ensures that the CPU will remain in Idle mode
   // until CRC1_IN DMA channel transfer is complete.
   do
   {
      #ifdef DMA_TRANSFERS_USE_IDLE
      PCON |= 0x01;                    // go to Idle mode
      #endif

   }  while((DMA0INT & CRC1_IN_MASK)==0);        // wait on DMA CH1 interrupt

   CRC1CN &= ~CRC1_DMA_ENABLE;         // disable CRC1

   // Clear CRC1_IN bit in DMA0EN sfr using mask.
   DMA0EN &= ~CRC1_IN_MASK;
   // Clear CRC1_IN bit in DMA0INT sfr using mask.
   DMA0INT &= ~CRC1_IN_MASK;

   value.U8[LSB] = CRC1OUTL;
   value.U8[MSB] = CRC1OUTH;

   //copy value to results location
   *results = value.U16;

   return SUCCESS;
}
//=============================================================================
// End of file
//=============================================================================

