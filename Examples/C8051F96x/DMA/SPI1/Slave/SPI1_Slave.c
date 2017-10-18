//=============================================================================
// SPI1_Master.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This reusable module includes API functions for SPI1 slave mode
// transfers using the DMA. Include the header file for this module
// to use the API functions.
//
// This module does not include the SPI1 init. These functions are
// located in main and must be copied to the user's main module.
//
// The DMA channels are configured in the DMA-defs.h header file.
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
//    C8051F960 Example Code
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <C8051F960_defs.h>
#include "SPI1_Slave.h"
#include "DMA_defs.h"
//=============================================================================
// API Varibles - externed in hader file
//=============================================================================
bit SPI_SlaveTransferDone;
//=============================================================================
// API Fuctions
//=============================================================================
//-----------------------------------------------------------------------------
// SPI1_SlaveInit
//-----------------------------------------------------------------------------
void SPI1_SlaveOutIn (U16 size,
   VARIABLE_SEGMENT_POINTER (slaveInPointer, U8, SEG_XDATA),
   VARIABLE_SEGMENT_POINTER (slaveOutPointer, U8, SEG_XDATA))
{
   UU16 addr;
   UU16 value;

   SFRPAGE = DPPE_PAGE;
   DMA0EN &= ~SPI1_MASK;

   // SPI1_IN means XRAM to SPI1DAT
   // This data is transmitted on MISO
   // Pointer is to Slave Out data
   DMA0SEL = SPI1_IN_CHANNEL;
   DMA0NCF = SPI1_IN_PERIPHERAL_REQUEST;
   DMA0NMD = NO_WRAPPING;
   addr.U16 = (U16)(slaveOutPointer);
   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];
   value.U16 = size;
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   // SPI1_OUT means SPI1DAT to XRAM
   // This data is received on MOSI
   // Pointer is to Slave In data
   DMA0SEL = SPI1_OUT_CHANNEL;
   DMA0NCF = SPI1_OUT_PERIPHERAL_REQUEST|DMA_INT_EN;
   DMA0NMD = NO_WRAPPING;
   addr.U16 = (U16)(slaveInPointer);
   DMA0NBAL = addr.U8[LSB];
   DMA0NBAH = addr.U8[MSB];
   value.U16 = size;
   DMA0NSZL = value.U8[LSB];
   DMA0NSZH = value.U8[MSB];
   DMA0NAOL = 0;
   DMA0NAOH = 0;

   DMA0INT &= ~SPI1_MASK;
   DMA0EN = SPI1_MASK;

   SPI_SlaveTransferDone = 0;

   EIE2 |= 0x20;                       // enable DMA interrupt

}
//-----------------------------------------------------------------------------
// DMA_ISR
// description:
//
//-----------------------------------------------------------------------------
INTERRUPT(DMA_ISR, INTERRUPT_DMA0)
{
  EIE2 &= ~0x20;                       // disable further interrupts

  SFRPAGE = DPPE_PAGE;

  if((DMA0INT & SPI1_OUT_MASK)==SPI1_OUT_MASK)
  {
     DMA0EN &= ~SPI1_MASK;
     DMA0INT &= ~SPI1_MASK;

     SPI_SlaveTransferDone = 1;

     // Add additional code here to process received data
     // and load next data into SPI1 Out buffer.
  }
  else
  {
     // add additional DMA interrupt code here
  }
}
//=============================================================================
// End
//=============================================================================

