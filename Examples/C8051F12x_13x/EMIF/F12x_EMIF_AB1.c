//-----------------------------------------------------------------------------
// F12x_EMIF_AB1.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program configures the external memory interface to read and write
// to an external SRAM (AB1).
//
// Pinout:
//
//    P0.0 - UART1 TX
//    P0.1 - UART1 RX
//
//    P4.4 - RAM_CS (RAM Chip Select)
//    P4.5 - RAM_BANK (RAM Bank Select)
//    P4.6 - /RD (EMIF)
//    P4.7 - /WR (EMIF)
//
//    P5, P6, P7 used for EMIF
//
//    all other port pins unused
//
// How To Test:
//
// 1) Load the F12x_EMIF_AB1.c in the Silicon Laboratories IDE.
// 2) Connect the AB1 board to the 'F12x TB (with jumper in place on AB1).
// 3) Verify UART jumpers are in place (J6 and J9).
// 4) Connect a serial cable from the DB9 connector on the 'F12x TB to the PC.
// 5) Open Hyperterminal (or a similar program) and connect to the board with
//    <BAUDRATE> and 8-N-1.
// 6) Compile and download code to a the 'F12x TB.
// 7) Run the code.
// 8) Verify the UART displays correct results in Hyperterminal.
//
//
// Target:         C8051F12x
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -16 OCT 2007
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F120.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F36x
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // SYSCLK frequency in Hz
#define BAUDRATE     115200            // Baud rate of UART in bps

#define RAM_SIZE     1024

sbit RAM_BANK = P4^5;                  // Bank select bit is P4^5
sbit RAM_CS = P4^4;                    // Chip select bit is P3^3

sbit LED = P1^6;                       // LED = 1 means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void UART1_Init (void);
void EMIF_Init (void);

//-----------------------------------------------------------------------------
// Global VARIABLES
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------

void main (void) 
{
   unsigned char xdata *pchar;         // Memory access pointer
   unsigned long i;

   WDTCN = 0xDE;                       // Disable Watchdog timer
   WDTCN = 0xAD;

   PORT_Init();                        // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   UART1_Init ();                      // Initialize UART0
   EMIF_Init ();                       // Initialize memory interface

   SFRPAGE = CONFIG_PAGE;              // P4 is on SFR Page F

   RAM_BANK = 0;                       // Select lower bank
   RAM_CS = 0;                         // Assert RAM chip select

   SFRPAGE = LEGACY_PAGE;

   // Clear xdata space
   pchar = 0x4000;
   for (i = 0; i < RAM_SIZE; i++) {
      *pchar++ = 0;
      // Print status to UART0
      if ((i % 16) == 0) {
         printf ("\nwriting 0x%04x: %02x ", (unsigned) i, (unsigned) 0);
      } else {
         printf ("%02x ", (unsigned) 0);
      }
   }

   // Verify all are zero
   pchar = 0x4000;
   for (i = 0; i < RAM_SIZE; i++) {
      if (*pchar != 0) {
         printf ("Erase error!\n");
         while (1);
      }
      // Print status to UART0
      if ((i % 16) == 0) {
         printf ("\nverifying 0x%04x: %02x ", (unsigned) i, (unsigned) *pchar);
      } else {
         printf ("%02x ", (unsigned) *pchar);
      }
      pchar++;
   }

   // Write xdata space
   pchar = 0x4000;
   for (i = 0; i < RAM_SIZE; i++) {
      *pchar = ~i;
      // Print status to UART0
      if ((i % 16) == 0) {
         printf ("\nwriting 0x%04x: %02x ", (unsigned) i, (unsigned) ((~i) & 0xff));
      } else {
         printf ("%02x ", (unsigned) ((~i) & 0xff));
      }
      pchar++;
   }

   // Verify
   pchar = 0x4000;
   for (i = 0; i < RAM_SIZE; i++) {
      if (*pchar != ((~i) & 0xff)) {
         printf ("Verify error!\n");
         while (1);
      }
      // Print status to UART0
      if ((i % 16) == 0) {
         printf ("\nverifying 0x%04x: %02x ", (unsigned) i, (unsigned) *pchar);
      } else {
         printf ("%02x ", (unsigned) *pchar);
      }
      pchar++;
   }

   printf ("\n\nTest complete.");

   while (1);
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
//    P0.0 - UART1 TX (push-pull)
//    P0.1 - UART1 RX
//
//    P4.4 - RAM_CS (RAM Chip Select) (push-pull)
//    P4.5 - RAM_BANK (RAM Bank Select) (push-pull)
//    P4.6 - /RD (EMIF) (push-pull)
//    P4.7 - /WR (EMIF) (push-pull)
//
//    P5, P6, P7 used for EMIF (push-pull)
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR2    = 0x44;                     // Enable UART1 on P0.0(TX) and P0.1(RX)
                                       // Enable crossbar and enable
                                       // weak pull-ups

   P0MDOUT |= 0x01;                    // Enable UTX as push-pull output
   P1MDOUT |= 0x40;                    // Enable LED as push-pull output

   // EMIF Initializations
   P4MDOUT |= 0xF0;                    // /WR, /RD, RAM_CS, and RAM_BANK are
                                       // push-pull
   P5MDOUT |= 0xFF;
   P6MDOUT |= 0xFF;
   P7MDOUT |= 0xFF;

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal oscillator
// at its maximum frequency.
//
// Also enables the Missing Clock Detector and VDD monitor as reset sources.
//
//-----------------------------------------------------------------------------
void SYSCLK_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency
   RSTSRC = 0x06;                      // Enable missing clock detector
                                       // and VDD monitor as reset sources

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <baudrate> and 8-N-1.
//
//-----------------------------------------------------------------------------
void UART1_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = UART1_PAGE;
   SCON1   = 0x10;                     // SCON1: mode 0, 8-bit UART, enable RX

   SFRPAGE = TIMER01_PAGE;
   TMOD   &= ~0xF0;
   TMOD   |=  0x20;                    // TMOD: timer 1, mode 2, 8-bit reload


   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON |= 0x10;                   // T1M = 1; SCA1:0 = xx
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x01;                  // T1M = 0; SCA1:0 = 01
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x13;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x13;                  // Clear all T1 related bits
      CKCON |=  0x02;                  // T1M = 0; SCA1:0 = 10
   }

   TL1 = TH1;                          // Initialize Timer1
   TR1 = 1;                            // Start Timer1

   SFRPAGE = UART1_PAGE;
   TI1 = 1;                            // Indicate TX1 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page

}

//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the external memory interface to non-multiplexed split-mode
// with bank select.
//
//-----------------------------------------------------------------------------
void EMIF_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = EMI0_PAGE;                // Switch to the necessary SFRPAGE

   EMI0CF = 0x38;                      // Non-muxed mode; split mode with bank
                                       // select
   EMI0CN = 0x40;                      // Access addresses on page 0x4000 for
                                       // 8-bit XRAM moves

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
//  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002
//
// putchar (basic version): expands '\n' into CR LF
//
//-----------------------------------------------------------------------------
char putchar (char c)
{
   char SFRPAGE_SAVE = SFRPAGE;

   SFRPAGE = UART1_PAGE;

   if (c == '\n')                      // If carriage return
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0d;                    // Output CR
   }

   while (!TI0);                       // Wait for transmit complete
   TI0 = 0;

   SBUF0 = c;                          // Send character

   SFRPAGE = SFRPAGE_SAVE;

   return c;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------