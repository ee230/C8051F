//-----------------------------------------------------------------------------
// F560_EMIF.c
//-----------------------------------------------------------------------------
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program configures the external memory interface to read and write
// to an external SRAM mapped to the upper port pins. The EMIF is configured
// for multiplexed mode.
//
// Resources Used:
// * Internal Oscillator
// * UART0
// * Timer 1: Used for UART
// * EMIF
//
// How To Test:
//
// 1) Connect the C8051F560-TB to an AB1 expansion board.  The AB1 expansion
//    board includes a 128K SRAM.
// 2) On the C8051F560-TB, remove the jumper on the J20 header to
//    disconnect the potentiometer from the port pin used for RAM CS
// 3) Download code to the side 'A' MCU on a C8051F560-TB
// 4) Connect the USB cable from the CP2102 USB-to-UART bridge on the
//    development board to a PC
// 5) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE>, 8 bits, no parity, 1 stop bit and no flow
//    control.
// 6) HyperTerminal will print the EMIF commands used
//
// Target: C8051F560
// Tool chain:     Raisonance / Keil / SDCC
// Command Line:   None
//
// Release 1.1 / 11 MAR 2010 (GP)
//    -Tested with Raisonance
//
// Release 1.0 / 15 JAN 2009 (GP)
//    - Initial Revision
//
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <compiler_defs.h>
#include <c8051f560_defs.h>            // SFR declarations
#include <stdio.h>                     // For printf()

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // Output of PLL derived from (INTCLK*2)
#define BAUDRATE       115200          // Baud rate of UART in bps
#define RAM_SIZE        2048          // Amount of ram to test

SBIT (RAM_BANK, SFR_P1, 4);            // Bank select bit is P1^4
SBIT (RAM_CS, SFR_P1, 2);              // Chip select bit is P1^2

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void main (void);

void SYSCLK_Init (void);
void PORT_Init (void);
void UART0_Init (void);
void EMIF_Init (void);

//-----------------------------------------------------------------------------
// main Routine
//-----------------------------------------------------------------------------

void main (void)
{
   // Declaring *pchar for the off-chip memory access.  Declaring the data
   // type as "xdata" forces the compiler to use MOVX
   U8 xdata *pchar;
   U32 i;                              // Loop index

   SFRPAGE = ACTIVE_PAGE;              // Set page for PCA and UART0

   PCA0MD &= ~0x40;                    // Disable the watchdog timer

   SYSCLK_Init ();                     // Initialize oscillator
   PORT_Init ();                       // Initialize crossbar and GPIO
   UART0_Init ();                      // Initialize UART0
   EMIF_Init ();                       // Initialize memory interface

   RAM_BANK = 0;                       // Select lower bank,
   RAM_CS = 0;                         // Assert RAM chip select

   //---- Set all bytes to zero
   pchar = 0;                          // Initialize XRAM pointer to first byte
   for (i = 0; i < RAM_SIZE; i++)
   {
      *pchar++ = 0;                    // Set RAM byte to 0

      // Print status to UART0
      if ((i % 16) == 0)
      {
         printf ("\nwriting 0x%04x: %02x ", (unsigned) i, (unsigned) 0);
      }
      else
      {
         printf ("%02x ", (unsigned) 0);
      }

   }

   //---- Verify all bytes are zero
   pchar = 0;                          // Initialize XRAM pointer to first byte
   for (i = 0; i < RAM_SIZE; i++)
   {
      if (*pchar != 0)
	   {
         printf ("Erase error!\n");
         while (1);
      }

      // Print status to UART0
      if ((i % 16) == 0)
      {
         printf ("\nverifying 0x%04x: %02x ", (unsigned) i, (unsigned) *pchar);
      }
      else
      {
         printf ("%02x ", (unsigned) *pchar);
      }

      pchar++;                         // Increment pointer
   }

   //---- Write xdata space with a sequence of values
   pchar = 0;
   for (i = 0; i < RAM_SIZE; i++)
   {
      *pchar = ~i;                     // Set XRAM byte to loop index

      // Print status to UART0
      if ((i % 16) == 0)
      {
         printf ("\nwriting 0x%04x: %02x ",
                  (unsigned)i, (unsigned)((~i) & 0xFF));
      }
      else
      {
         printf ("%02x ", (unsigned) ((~i) & 0xFF));
      }

      pchar++;                         // Increment pointer
   }

   //---- Verify the sequence of values
   pchar = 0;
   for (i = 0; i < RAM_SIZE; i++)
   {
      if (*pchar != ((~i) & 0xFF))
	   {
         printf ("Verify error!\n");
         while (1);
      }

      // Print status to UART0
      if ((i % 16) == 0)
      {
         printf ("\nverifying 0x%04x: %02x ", (unsigned) i, (unsigned) *pchar);
      }
      else
      {
         printf ("%02x ", (unsigned) *pchar);
      }

      pchar++;
   }

   printf ("\n\nTest passed.");
   while (1);
}

//-----------------------------------------------------------------------------
// Initialization Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at 24 MHz.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   U8 SFRPAGE_SAVE = SFRPAGE;          // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x87;                      // Set internal oscillator to run
                                       // at its maximum frequency

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initialized the GPIO and the Crossbar
//
// P0.4   digital   push-pull    UART TX
// P0.5   digital   open-drain   UART RX
//
// P1.2   digital   push-pull    RAM_CS
// P1.4   digital   push-pull    RAM Bank (A16)
// P1.5   digital   push-pull    ALE
// P1.6   digital   push-pull    /RD
// P1.7   digital   push-pull    /WR
//
// P2     digital   push-pull    Address pins A8-15
// P3     digital   push-pull    Address pins A0-7 / D0-7
//
//-----------------------------------------------------------------------------

void PORT_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;          // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   P0MDOUT |= 0x10;                    // Set UART TX push-pull
   P1MDOUT |= 0xF4;                    // Set EMIF pins to push-pull
   P2MDOUT |= 0xFF;                    // Set EMIF pins to push-pull
   P3MDOUT |= 0xFF;                    // Set EMIF pins to push-pull
   P4MDOUT |= 0xFF;                    // Set EMIF pins to push-pull

   XBR0 = 0x01;                        // Enable UART I/O
   XBR2 = 0x40;                        // Enable crossbar

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 for <BAUDRATE> and 8-N-1.
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   U8 SFRPAGE_save = SFRPAGE;
   SFRPAGE = CONFIG_PAGE;

 // Baud Rate = [BRG Clock / (65536 - (SBRLH0:SBRLL0))] x 1/2 x 1 / Prescaler

#if   ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 1)
      SBRL0 = -(SYSCLK / BAUDRATE / 2);
      SBCON0 |= 0x03;                  // Set prescaler to 1
#elif ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 4)
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 4);
      SBCON0 &= ~0x03;
      SBCON0 |= 0x01;                  // Set prescaler to 4
#elif ((SYSCLK / BAUDRATE / 2 / 0xFFFF) < 12)
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 12);
      SBCON0 &= ~0x03;                 // Set prescaler to 12
#else
      SBRL0 = -(SYSCLK / BAUDRATE / 2 / 48);
      SBCON0 &= ~0x03;
      SBCON0 |= 0x02;                  // Set prescaler to 48
#endif

   SBCON0 |= 0x40;                     // Enable baud rate generator

   SFRPAGE = ACTIVE_PAGE;

   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       // clear RI0 and TI0 bits

   TI0 = 1;                            // Indicate TX0 ready

   SFRPAGE = SFRPAGE_save;
}

#ifdef SDCC

// SDCC does not include a definition for putchar(), which is used in printf()
// and so it is defined here.  The prototype does not need to be explicitly
// defined because it is provided in stdio.h

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : character to send to UART
//
// This function outputs a character to the UART.
//-----------------------------------------------------------------------------
void putchar (char output)
{
   if (output == '\n')
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0D;
   }
   while (!TI0);
   TI0 = 0;
   SBUF0 = output;
}

#endif

//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the external memory interface to use upper port pins in
// non-multiplexed mode to a mixed on-chip/off-chip configuration without
// Bank Select.
//
//-----------------------------------------------------------------------------
void EMIF_Init (void)
{
   U8 SFRPAGE_SAVE = SFRPAGE;          // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Save SFR_PAGE

   EMI0CF = 0x0C;                      // All MOVX instructions access off-chip

   EMI0TC = 0x00;                      // Fastest timing

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR_PAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
