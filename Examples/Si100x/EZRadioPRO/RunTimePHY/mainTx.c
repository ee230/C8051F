//=============================================================================
// mainTx.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//  This file is specific to the Si100x hardware.
//
// Target:
//    Si1000
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
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
void PortInit (void);
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

   PCA0MD   &= ~0x40;                  // disable F930 watchdog
   // Configure DC/DC
   // Needed for Si1004/5 support only
#ifdef SI1004_DAUGHTER_CARD
   DC0CN = 0x43;                       // 20ns min, big SW, 2.1V
   DC0CF = 0x04;                       // Local osc, High-Peak Current
#endif

   SYSCLK_Init();
   SPI_Init();
   PortInit();

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
      if(SW02==0)
      {
         LED15 = ILLUMINATE;
         status = rtPhyTx(64,TxBuffer);
         if(status==PHY_STATUS_SUCCESS)
            LED15 = EXTINGUISH;
         while(SW02==0);
      }
   }
}
//=============================================================================
//
// Init Functions
//
//=============================================================================
//-----------------------------------------------------------------------------
// PortInit
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and ports pins.
//
// P1.0  -  SCK1  push-pull   high-current   digital output
// P1.1  -  MISO1 open-drain                 digital inpout
// P1.2  -  MOSI1 push-pull   high-current   digital output
// P1.4  -  NSS1  push-pull   high-current   digital output
//
// P2.6  -  SDN   push-pull     Debug Pin
//
//-----------------------------------------------------------------------------
void PortInit (void)
{
   // Init SPI
   XBR1    |= 0x40;                     // Enable SPI1 (3 wire mode)
   P1MDOUT |= 0x15;                    // SCK1, MOSI1, & NSS1,push-pull
   SFRPAGE   = CONFIG_PAGE;
   P1DRV     = 0x15;                   // MOSI1, SCK1, NSS1, high-current mode
   SFRPAGE   = LEGACY_PAGE;

   // Initialize SDN
#ifdef   SI1000_DAUGHTER_CARD
   P2MDOUT |= 0x40;                    // SDN P2.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV   |= 0x40;                    // SDN  P2.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
#ifdef   SI1002_DAUGHTER_CARD
   P2MDOUT |= 0x40;                    // SDN P2.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P2DRV   |= 0x40;                    // SDN  P2.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
#ifdef   SI1004_DAUGHTER_CARD
   P1MDOUT |= 0x40;                    // SDN P1.6 push-pull
   SFRPAGE  = CONFIG_PAGE;
   P1DRV   |= 0x40;                    // SDN  P1.6 high-current mode
   SFRPAGE  = LEGACY_PAGE;
#endif
   SDN = 0;

   XBR2    |= 0x40;                     // Enable Crossbar

   IT01CF    = 0x01;                   // INT0 P0.1
   IE0 = 0;
   IT0 =0;
}
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
    CLKSEL    = 0x14;                  // Use 10 MHz LPOSC
}
//-----------------------------------------------------------------------------
void SPI_Init (void)
{
   // Init SPI
   SPI1CFG   = 0x40;                   // master mode
   SPI1CN    = 0x00;                   // 3 wire master mode
   SPI1CKR   = 0x00;                   // initialize SPI prescaler
   SPI1CN   |= 0x01;                   // enable SPI
   NSS1 = 1;                            // set NSS high
}
//=============================================================================
