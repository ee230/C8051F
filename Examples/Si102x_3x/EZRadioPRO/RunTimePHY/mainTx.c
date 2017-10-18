//=============================================================================
// mainTx.c
//-----------------------------------------------------------------------------
// Copyright 2011 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//  This file is specific to the Si102x/3x hardware.
//
// Target:
//    Si102x/3x
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
//    Run-Time PHY
//
// This software must be used in accordance with the End User License Agreement.
//
//=============================================================================
//-----------------------------------------------------------------------------
// Includes
//
// These includes must be in a specific order. Dependencies listed in comments.
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include "hardware_defs.h"             // requires compiler_defs.h
#include "rtPhy.h"
#include "rtPhy_const.h"

//-----------------------------------------------------------------------------
SEGMENT_VARIABLE (TxBuffer[64], U8, BUFFER_MSPACE);

//-----------------------------------------------------------------------------
// Internal Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init (void);
void SYSCLK_Init (void);
void SPI_Init (void);

//=============================================================================
// Functions
//=============================================================================
//-----------------------------------------------------------------------------
// main ()
//-----------------------------------------------------------------------------
void main (void)
{
   U8 status;
   U8 i;

   PCA0MD   &= ~0x40;                  // disable watchdog timer

   SYSCLK_Init();
   SPI_Init();
   Port_Init();

   status = rtPhyInit();

   // Set the TRX_FREQUENCY first
   rtPhySet (TRX_FREQUENCY, 915000000);

   // Set the TRX_DATA_RATE second
   rtPhySet (TRX_DATA_RATE, 40000L);

   // Set the TRX_DEVIATION third (optional)
   rtPhySet (TRX_DEVIATION, 20000L);

   // Set the RX_BAND_WIDTH last (optional)
   rtPhySet (RX_BAND_WIDTH, 80000L);

   // If desired, also set the TRX_CHANNEL_SPACING
   // rtPhySet (TRX_CHANNEL_SPACING, 100000L);

   status = rtPhyInitRadio();

   for (i=0;i<64;i++)
   {
      TxBuffer[i] = i;
   }

   while(1)
   {
      if(SW1==0)
      {
         LED2 = ILLUMINATE;
         status = rtPhyTx(64,TxBuffer);
         if(status==PHY_STATUS_SUCCESS)
            LED2 = EXTINGUISH;
         while(SW1==0);
      }
   }
}
//=============================================================================
//
// Init Functions
//
//=============================================================================
//-----------------------------------------------------------------------------
// Port_Init ()
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   U8 SFRPAGE_restore;   
   SFRPAGE_restore = SFRPAGE;   

   // Init SPI
   SFRPAGE  = LEGACY_PAGE;
   XBR1    |= 0x40;                    // Enable SPI1 (3 wire mode)
   P2MDOUT |= 0x0D;                    // SCK1, MOSI1, & NSS1,push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV    = 0x0D;                    // MOSI1, SCK1, NSS1, high-drive mode

   // Initialize SDN
   SFRPAGE  = LEGACY_PAGE;
   P1MDOUT |= 0x80;                    // SDN P1.7 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P1DRV   |= 0x80;                    // SDN P1.7 high-current mode
   SFRPAGE  = LEGACY_PAGE;
   SDN      = 0;

   P2       = 0xFF;                    // P2 bug fix for SDCC and Raisonance

   XBR2    |= 0x40;                    // enable Crossbar

   IT01CF   = 0x06;                    // INT0 is assigned to P1.6
   IE0      = 0;
   IT0      = 0;
   EX0      = 1;
  
   SFRPAGE  = SFRPAGE_restore;
}

//-----------------------------------------------------------------------------
// SYSCLK_Init ()
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 SFRPAGE_restore;   
   SFRPAGE_restore = SFRPAGE;
   
   SFRPAGE = LEGACY_PAGE;
   CLKSEL    = 0x14;                   // Use 10 MHz LPOSC

   SFRPAGE  = SFRPAGE_restore;
}

//-----------------------------------------------------------------------------
// SPI1_Init ()
//-----------------------------------------------------------------------------
void SPI_Init (void)
{
   U8 SFRPAGE_restore;   
   SFRPAGE_restore = SFRPAGE;
   
   SFRPAGE = SPI1_PAGE;
   SPI1CFG  |= 0x40;                   // master mode
   SPI1CN    = 0x00;                   // 3 wire master mode
   SPI1CKR   = 0x00;                   // initialize SPI prescaler
   SPI1CN   |= 0x01;                   // enable SPI
   NSS1 = 1;                           // set NSS high
   
   SFRPAGE  = SFRPAGE_restore;
}

//=============================================================================
