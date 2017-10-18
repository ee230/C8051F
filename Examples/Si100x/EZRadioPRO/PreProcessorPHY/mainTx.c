//=============================================================================
// mainTx.c
//-----------------------------------------------------------------------------
// Copyright 2010 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// C File Description:
//
//
// Target:
//    Si10xx
//
// IDE:
//    Silicon Laboratories IDE
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
//    PreProcessorPHY
//
// Release 1.1
//    - TBD
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
#include "ppPhy.h"
#include "ppPhy_const.h"
//-----------------------------------------------------------------------------
// Check for hardware definition on compiler command line
//-----------------------------------------------------------------------------
#ifndef SI1000_DEFS_H
#error "Please define si100x hardware on compiler command line"
#endif
//-----------------------------------------------------------------------------
// Tx Buffer
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

   PCA0MD   &= ~0x40;                  // disable F930 watchdog
   // Configure DC/DC
   // Needed for Si1004/5 support only
#ifdef SI1004_DAUGHTER_CARD
   DC0CN = 0x43;                       // 20ns min, big SW, 2.1V
   DC0CF = 0x04;                       // Local osc, High-Peak Current
#endif

   SYSCLK_Init();
   SPI_Init();
   Port_Init();

   status = ppPhyInit();
   status = ppPhyInitRadio();

   for (i=0;i<64;i++)
   {
      TxBuffer[i] = i;
   }

   while(1)
   {
      if(SW02==0)
      {
         LED15 = ILLUMINATE;
         status = ppPhyTx(64,TxBuffer);
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
// PORT_Init
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
void Port_Init (void)
{
   // Init SPI
   XBR1    |= 0x40;                    // Enable SPI1 (3 wire mode)
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

   P2 = 0xFF;                          // P2 bug fix for SDCC and Raisonance

   XBR2    |= 0x40;                    // Enable Crossbar

   IT01CF    = 0x01;                   // INT0 P0.1
   IE0 = 0;
   IT0 =0;
}

//------------------------------------------------------------------------------------------------
void SYSCLK_Init (void)
{
    CLKSEL    = 0x14;                  // Use 10 MHz LPOSC
}
//------------------------------------------------------------------------------------------------

void SPI_Init (void)
{
   // Init SPI
   SPI1CFG   = 0x40;                   // master mode
   SPI1CN    = 0x00;                   // 3 wire master mode
   SPI1CKR   = 0x00;                   // initialize SPI prescaler
   SPI1CN   |= 0x01;                   // enable SPI
   NSS1 = 1;                           // set NSS high
}
//===============================================================================================
