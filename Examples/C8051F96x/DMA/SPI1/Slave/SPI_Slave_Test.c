//=============================================================================
// SPI_SlaveTest.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
// This file will test the SPI1 Slave mode using the DMA.
//
// To use connect P0.1-P0.3 to P2.0-P2.3. This will connect SPI1 to SPI0.
// SPI1 is used in slave mode with the DMA. SPI0 is used in Master mode
// using polling.
//
// The MISO data is copied to XRAM for the DMA. This data will be sent
// out by the slave as the the Slave receives MOSI data.
//
// The SPI1_SlaveNext function will prepare the DMA for the next SPI
// transfer.
//
// Then SPI0 is used to send data over the SPI connection. After sending
// all the MOSI data bytes, the DMA interrupt should occur.
//
// After a successful SPI transfer the MOSI data should be in the
// SPI1_SlaveInBuffer and the MISO data should be in the
// SPI0_MasterInBuffer
////
//
// Target:
//    C8051F960 "Metron"
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
//    Metron DPPE Validation
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
//-----------------------------------------------------------------------------
// DEFINE HARDWARE BITS
//-----------------------------------------------------------------------------
SBIT(NSS0, SFR_P0, 3);
//=============================================================================
// Test Packet
//=============================================================================
#define TEST_PACKET_LENGTH 16
const SEGMENT_VARIABLE (MISO_Data[TEST_PACKET_LENGTH], U8, SEG_CODE) =
{
   0xA0, 0xA1, 0xA2, 0xA3,
   0xA4, 0xA5, 0xA6, 0xA7,
   0xA8, 0xA9, 0xAA, 0xAB,
   0xAC, 0xAD, 0xAE, 0xAF,
};

const SEGMENT_VARIABLE (MOSI_Data[TEST_PACKET_LENGTH], U8, SEG_CODE) =
{
   0xBF, 0xBE, 0xBD, 0xBC,
   0xBB, 0xBA, 0xB9, 0xB8,
   0xB7, 0xB6, 0xB5, 0xB4,
   0xB3, 0xB2, 0xB1, 0xB0,
};

//=============================================================================
// File global variables
//=============================================================================
//-----------------------------------------------------------------------------
// Global arrays for data stored in xram
//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (SPI1_SlaveOutBuffer[TEST_PACKET_LENGTH], U8, SEG_XDATA);
SEGMENT_VARIABLE (SPI1_SlaveInBuffer[TEST_PACKET_LENGTH], U8, SEG_XDATA);
SEGMENT_VARIABLE (SPI0_MasterInBuffer[TEST_PACKET_LENGTH], U8, SEG_XDATA);
//=============================================================================
// Function Prototypes
//=============================================================================void
void SYSCLK_Init (void);
void SPI_Init(void);
void PortInit(void);


void memCopyCX(U16 n,
   const VARIABLE_SEGMENT_POINTER (cPointer, U8, SEG_CODE),
   VARIABLE_SEGMENT_POINTER (xPointer, U8, SEG_XDATA));

void SPI0_MasterOutIn (U16 length,
   const VARIABLE_SEGMENT_POINTER (writePointer, U8, SEG_CODE),
   VARIABLE_SEGMENT_POINTER (readPointer, U8, SEG_XDATA));

U16 countErrors (U16 n,
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
   U16 errors;

   PCA0MD  &= ~0x40;                   // disable watchdog timer

   SYSCLK_Init();
	SPI_Init();
   PortInit();

   EA = 1;

   memCopyCX(TEST_PACKET_LENGTH, MISO_Data, SPI1_SlaveOutBuffer);

   SPI1_SlaveOutIn(TEST_PACKET_LENGTH, SPI1_SlaveInBuffer, SPI1_SlaveOutBuffer);

   SPI0_MasterOutIn(TEST_PACKET_LENGTH, MOSI_Data, SPI0_MasterInBuffer);

   while(!SPI_SlaveTransferDone);

   errors = countErrors(TEST_PACKET_LENGTH, SPI1_SlaveInBuffer, MOSI_Data);

   while(errors);

   errors = countErrors(TEST_PACKET_LENGTH, SPI0_MasterInBuffer, MISO_Data);

   while(errors);

   while(1)
   {
   }
}
//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 24.5 MHz
// oscillator as its clock source.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{

    SFRPAGE = LEGACY_PAGE;
    OSCICN |= 0x80;
    CLKSEL    = 0x00;
}
//-----------------------------------------------------------------------------
// SPI_Init()
//-----------------------------------------------------------------------------
void SPI_Init(void)
{
   SFRPAGE = LEGACY_PAGE;
   SPI0CFG   = 0x40;  // 3-wire master mode
   SPI0CN    = 0x01;
   SPI0CKR   = 0x04;  // SYSCLK/10 - Maximum for Slave Mode


   SFRPAGE = SPI1_PAGE;
   SPI1CFG   = 0x00;
   SPI1CN    = 0x05;
   SPI1CKR   = 0x00;
}
//-----------------------------------------------------------------------------
// PortInit()
//-----------------------------------------------------------------------------
void PortInit(void)
{
   SFRPAGE = LEGACY_PAGE;

   XBR0      = 0x02; // enable SP0
   XBR1      = 0x40; // enable SPI1
   XBR2      = 0x40; // enable crossbar

   P0MDOUT   = 0x0D;
   P2MDOUT   = 0x02;

   SFRPAGE = CONFIG_PAGE;
   P0DRV   = 0x0D;
   P2DRV   = 0x02;

}

//-----------------------------------------------------------------------------
//  SPI0_MasterOutIn ()
//-----------------------------------------------------------------------------

void SPI0_MasterOutIn (U16 length,
   const VARIABLE_SEGMENT_POINTER (writePointer, U8, SEG_CODE),
   VARIABLE_SEGMENT_POINTER (readPointer, U8, SEG_XDATA))
{

   NSS0 = 0;

   SFRPAGE = LEGACY_PAGE;
   do
   {
      SPIF0 = 0;
      SPI0DAT = *writePointer++;
      while(!SPIF0);
      *readPointer++ = SPI0DAT;
  } while(--length);

  NSS0 = 1;
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

