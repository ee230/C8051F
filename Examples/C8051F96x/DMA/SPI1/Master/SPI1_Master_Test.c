//=============================================================================
// SPI1_Master_Test.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file uses the SPI1_Master.c API functions with SPI1 and the DMA.
// The code in main facilitates a loopback test to demonstrate SPI transfers.
//
// To use P2.1/MOSI must be shorted to P2.2/MISO.
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
//    F960 Example Code
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051F960_defs.h>
#include "SPI1_Master.h"
#include "DMA_defs.h"
//=============================================================================
// code constants
//=============================================================================
//-----------------------------------------------------------------------------
// Test Packet Length
//-----------------------------------------------------------------------------
#define TEST_PACKET_LENGTH 64
//-----------------------------------------------------------------------------
// Test Packet
//-----------------------------------------------------------------------------
const SEGMENT_VARIABLE (TestPacket[TEST_PACKET_LENGTH], U8, SEG_CODE) =
{
   0x00, 0x41, 0x82, 0xC3,
   0x04, 0x45, 0x86, 0xC7,
   0x08, 0x49, 0x8A, 0xCB,
   0x0C, 0x4D, 0x8E, 0xCF,
   0x10, 0x51, 0x92, 0xD3,
   0x14, 0x55, 0x96, 0xD7,
   0x18, 0x59, 0x9A, 0xDB,
   0x1C, 0x5D, 0x9E, 0xDF,
   0x20, 0x61, 0xA2, 0xE3,
   0x24, 0x65, 0xA6, 0xE7,
   0x28, 0x69, 0xAA, 0xEB,
   0x2C, 0x6D, 0xAE, 0xEF,
   0x30, 0x71, 0xB2, 0xF3,
   0x34, 0x75, 0xB6, 0xF7,
   0x38, 0x79, 0xBA, 0xFB,
   0x3C, 0x7D, 0xBE, 0xFF
};
//=============================================================================
// File global variables
//=============================================================================
//-----------------------------------------------------------------------------
// Global arrays for data stored in xram
//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (SPI1_OutBuffer[256], U8, SEG_XDATA);
SEGMENT_VARIABLE (SPI1_InBuffer[256], U8, SEG_XDATA);
//=============================================================================
// Function Prototypes
//=============================================================================
void SPI1_Init(void);
void PortInit(void);

void memCopyCX(U16 n,
   const VARIABLE_SEGMENT_POINTER (cPointer, U8, SEG_CODE),
   VARIABLE_SEGMENT_POINTER (xPointer, U8, SEG_XDATA));

U16 countErrors (U16 bytes,
   VARIABLE_SEGMENT_POINTER (results, U8, SEG_XDATA),
   const VARIABLE_SEGMENT_POINTER (expectedResults, U8, SEG_CODE));
//-----------------------------------------------------------------------------
// Interrupt proto (for SDCC compatibility)
//-----------------------------------------------------------------------------
INTERRUPT_PROTO(DMA_ISR, INTERRUPT_DMA0);

//=============================================================================
// main
//=============================================================================
void main (void)
{
   U8 errors;

   PCA0MD  &= ~0x40;                   // disable watchdog timer

   SPI1_Init();
   PortInit();

   memCopyCX(TEST_PACKET_LENGTH, TestPacket, SPI1_OutBuffer);

   EA = 1;

   SPI1_MasterOutIn (TEST_PACKET_LENGTH, SPI1_OutBuffer, SPI1_InBuffer);

   errors = countErrors (TEST_PACKET_LENGTH, SPI1_InBuffer, TestPacket);

   while(errors);

   // The write only function is not tested by code, but can be observed on an oscilloscope.
   SPI1_MasterOutOnly (TEST_PACKET_LENGTH, SPI1_OutBuffer);

   while(1);
}
//-----------------------------------------------------------------------------
// SPI1_Init
//-----------------------------------------------------------------------------
void SPI1_Init(void)
{
   SFRPAGE = SPI1_PAGE;
   SPI1CFG   = 0x40;
   SPI1CN    = 0x01;
   SPI1CKR   = 0x00;
}
//-----------------------------------------------------------------------------
// SPI1_Init
//-----------------------------------------------------------------------------
void PortInit(void)
{
   SFRPAGE = LEGACY_PAGE;

   XBR1      = 0x40; // enable SPI1
   XBR2      = 0x40; // enable crossbar

   SFRPAGE = LEGACY_PAGE;
   P2MDOUT   = 0x05;
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
// memCopyCX()
//
// parameters:
//    U8 n - number of bytes to copy
//
// This function copies the Reference plain Text to the global xram variable.
//
//-----------------------------------------------------------------------------
void memCopyCX(U16 n,
   const VARIABLE_SEGMENT_POINTER (cPointer, U8, SEG_CODE),
   VARIABLE_SEGMENT_POINTER (xPointer, U8, SEG_XDATA))
{
   do
   {
      *xPointer++ = *cPointer++;
   } while (--n);
}
//-----------------------------------------------------------------------------
// countErrors()
//
// parameters:
//-----------------------------------------------------------------------------
U16 countErrors (U16 bytes,
   VARIABLE_SEGMENT_POINTER (results, U8, SEG_XDATA),
   const VARIABLE_SEGMENT_POINTER (expectedResults, U8, SEG_CODE))
{
   U16 errors;

   errors = 0;

   while(bytes)
   {
      if(*results++ != *expectedResults++)
         errors++;
      bytes--;
   }

   return errors;
}

//=============================================================================
// End
//=============================================================================

