//=============================================================================
// SPI_PassThrough_Si1004.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This pass though code is used to permit the Si1000 MCM to be used with
// existing EZRadio Pro test code. The Two SPIs are configured one with one
// Slave and one Master. The Slave SPI (SPI0) emulates the EZRadio interface
// and the Master SPI communicates with the Si1000 radio. This provides nearly
// transparent SPI pass through.
//
// The Slave SPI does have some limitations. The data rate should be limited
// to 1 MHz. Additionally, there are following timing restrictions.
//
// Parameter   Min   Max   Units
// fSCK        —     1     MHz
// TSCK        1.0   —     microseconds
// TNC         1.0   —     microseconds
// TCN         1.5   —     microseconds
// TNH         1.0   —     microseconds
// TW          1.5   —     microseconds
// TR          4.5   —     microseconds
//
//
// Parameter   Description
// fSCK        SCK Frequency
// TSCK        SCK Period
// TNC         NSS Falling to First SCK Edge
// TCN         Last SCK falling Edge to NSS Rising edge
// TNH         NSS High Time between transfers
// TW          Write transfer inter-byte delay SCLK falling edge to rising edge
// TR          Read transfer inter-byte delay SCLK falling edge to rising edge
//
// The SPI transfers are limited to 2 byte transfers - address and data.
// Multiple-byte transfers are not supported.
//
// Note that this code will drive the SDN pin low. This assumes the radio SDN
// pin is connected to P2.6 on the MCU. Of course, the pass through code will
// not work if the radio is in shutdown mode.
//
// FID:
// Target:
// Tool chain:     Keil C51
//
// Release 1.0
//    -Initial Revision (KAB)
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <Si1000_defs.h>
//-----------------------------------------------------------------------------
// hardware bit definitions
//-----------------------------------------------------------------------------
SBIT(NSS1, SFR_P1, 4);
SBIT(SDN, SFR_P1, 6);
//-----------------------------------------------------------------------------
// main () Routine
//-----------------------------------------------------------------------------
void main (void)
{
   U8 c;

   PCA0MD  &= ~0x40;                   // disable watchdog timer

   FLSCL     = 0x40;                   // enable FLSCL
   OSCICN    = 0x8F;                   // enable Precision Oscillator
   CLKSEL    = 0x00;                   // SYSCLK = 24.5 MHz

   // P1.0  -  SCK1  (SPI1) Push-Pull,  Digital
   // P1.1  -  MISO1 (SPI1) Open-Drain, Digital
   // P1.2  -  MOSI1 (SPI1) Push-Pull,  Digital
   //
   // P1.4  -  NSS1         Push-Pull,  Digital
   //
   // P2.0  -  SCK  (SPI0), Open-Drain, Digital
   // P2.1  -  MISO (SPI0), Push-Pull,  Digital
   // P2.2  -  MOSI (SPI0), Open-Drain, Digital
   // P2.3  -  NSS  (SPI0), Open-Drain, Digital
   //
   // P1.6  -  SDN  (SPI0), Push-Pull,  Digital

   P0SKIP    = 0xFF;
   P1SKIP    = 0xF8;
   P1MDOUT   = 0x05;                   // SCK1, MOSI1, NSS1 push-pull
   P2MDOUT   = 0x02;                   // MISO push pull
   SFRPAGE   = CONFIG_PAGE;
   P1DRV     = 0x05;
   P2DRV     = 0x02;
   SFRPAGE   = LEGACY_PAGE;

   P1MDOUT   |= 0x10;
   SFRPAGE   = CONFIG_PAGE;
   P1DRV     |= 0x10;
   SFRPAGE   = LEGACY_PAGE;

   P2MDOUT   |= 0x40;                  // SCK1, MOSI1, NSS1 push-pull
   SFRPAGE   = CONFIG_PAGE;
   P2DRV     |= 0x40;
   SFRPAGE   = LEGACY_PAGE;

   XBR0      = 0x02;                   // enable SPI0
   XBR1      = 0x40;                   // enable SPI1
   XBR2      = 0x40;                   // enable crossbar

   NSS1 = 1;                           // set NSS high

   SDN = 0;

   SPI0CKR   = 0x00;                   // clear SPI0CKR, SPI0 in slave mode
   SPI0CFG   = 0x00;                   // disable master mode
   SPI0CN    = 0x05;                   // 4-wire mode, enable SPI0

   SPI1CKR   = 0x00;                   // use SYSCLK/2 (12.25 MHZ)
   SPI1CFG   = 0x40;                   // enable master mode
   SPI1CN    = 0x01;                   // 3 wire mode, enable SPI1

   SPIF0 = 0;
   SPIF1 = 0;
   SPI0DAT = 0;

   while (1)
   {
      while((SPI0CFG &0x08) == 0x00);  // wait on SLVSEL
      while (!SPIF0);                  // wait on first byte
      c = SPI0DAT;                     // read first byte

      if((c & 0x80)==0x80)             // Write operation
      {
         SPI0DAT = 0;                  // writes return zeros
         SPIF0 = 0;                    // clear slave SPIF

         NSS1 = 0;                     // Start master transfer
         SPIF1 = 0;                    // clear SPIF
         SPI1DAT = c;                  // Write first byte
         while(!SPIF1);                // wait on master SPIF

         while(!SPIF0);                // wait for next byte
         c = SPI0DAT;                  // read next byte
         SPI0DAT = 0;                  // clear slave SPIDAT
         SPIF0 = 0;                    // clear slave SPIF

         SPIF1 = 0;                    // clear master SPIF
         SPI1DAT = c;                  // write next byte
         while(!SPIF1);                // wait on SPIF

         NSS1 = 1;                     // Master transfer done
      }
      else                             // Read Operation
      {
         NSS1 = 0;                     // Start master transfer

         SPIF1 = 0;                    // clear master SPIF
         SPI1DAT = c;                  // send read address
         while(!SPIF1);                // wait on SPIF

         SPIF1 = 0;                    // get value ASAP
         SPI1DAT = 0x00;               // send dummy byte to get value
         while(!SPIF1);                // wait on value
         c = SPI1DAT;                  // read value
         NSS1 = 1;                     // end master transfer

         SPI0DAT = c;                  // send value on slave bus
         SPIF0 = 0;                    // clear slave SPIF

         while(!SPIF0);                // wait on slave transfer
         c = SPI0DAT;                  // dummy read
         SPI0DAT = 0;                  // clear DAT
         SPIF0 = 0;                    // clear SPIF
     }
     while((SPI0CFG &0x08) == 0x08);   // wait SLVSEL
   }
}
//=============================================================================
// End

//=============================================================================
