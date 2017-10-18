//-----------------------------------------------------------------------------
// F36x_EMIF.c
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
//    P1, P2, upper half of P3, and P4 used for EMIF
//
//    all other port pins unused
//
// How To Test:
//
// 1) Load the F36x_EMIF.c in the Silicon Laboratories IDE.
// 2) Connect the AB1 board to the 'F36x TB.  Verify that the jumper
//    on the AB1 is shorted.
// 3) Verify the P3.3 LED jumper is not populated on J12.
// 4) Verify all port pin jumpers are removed (e.g. J13, J15, J16, etc.).
// 5) Connect a USB cable from the USB connector on the 'F36x TB to the PC.
// 6) Open Hyperterminal (or a similar program) and connect to the board with
//    <BAUDRATE> and 8-N-1 (CP210x VCP drivers must be installed)
// 7) Compile and download code to a the 'F36x TB.
// 8) Run the code.
// 9) Verify the UART displays correct results in Hyperterminal.
//
//
// FID:            36X000011
// Target:         C8051F36x
// Tool chain:     KEIL C51 7.20 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (TP)
//    -24 OCT 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F360.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F36x
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

#define SYSCLK       24500000          // SYSCLK frequency in Hz
#define BAUDRATE     115200            // Baud rate of UART in bps

#define RAM_SIZE     5000

sbit RAM_BANK = P0^0;                  // Bank select bit is P0^0
sbit RAM_CS = P3^3;                    // Chip select bit is P3^3

sbit LED = P3^2;                       // LED = 1 means ON

//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------

void SYSCLK_Init (void);
void PORT_Init (void);
void UART0_Init (void);
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
   unsigned char reset_source;

   PCA0MD &= ~0x40;                    // WDTE = 0 (clear watchdog timer
                                       // enable)

   PORT_Init();                        // Initialize Port I/O
   SYSCLK_Init ();                     // Initialize Oscillator
   UART0_Init ();                      // Initialize UART0
   EMIF_Init ();                       // Initialize memory interface

   SFRPAGE = LEGACY_PAGE;

   RAM_BANK = 0;                       // Select lower bank
   RAM_CS = 0;                         // Assert RAM chip select

   printf ("\nRSTSRC = %d\n", reset_source);

   // Clear xdata space
   pchar = 0;
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
   pchar = 0;
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
   pchar = 0;
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
   pchar = 0;
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
// P0.1 - UART TX
// P0.2 - UART RX
// P3.2 - LED
//
void PORT_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR0    = 0x01;                     // Enable UART on P0.1(RX) and P0.2(TX)
   XBR1    = 0x40;                     // Enable crossbar and enable
                                       // weak pull-ups
   P0MDOUT |= 0x02;                    // Enable UTX as push-pull output
   P3MDOUT |= 0x04;                    // Enable LED as push-pull output

   P4MDOUT |= 0x30;                    // /WR and /RD are push-pull
   P4 |= 0x30;

   P1MDOUT |= 0xFF;
   P2MDOUT |= 0xFF;
   P3MDOUT |= 0xF0;
   P4MDOUT |= 0x0F;

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
// Also enables the Missing Clock Detector and VDD monitor.
//
void SYSCLK_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   OSCICN |= 0x03;                     // Configure internal oscillator for
                                       // its maximum frequency
   RSTSRC = 0x06;                      // Enable missing clock detector
                                       // and VDD monitor

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART0 using Timer1, for <BAUDRATE> and 8-N-1.
//
void UART0_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   SCON0 = 0x10;                       // SCON0: 8-bit variable bit rate
                                       //        level of STOP bit is ignored
                                       //        RX enabled
                                       //        ninth bits are zeros
                                       //        clear RI0 and TI0 bits
   if (SYSCLK/BAUDRATE/2/256 < 1) {
      TH1 = -(SYSCLK/BAUDRATE/2);
      CKCON &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
      CKCON |=  0x08;
   } else if (SYSCLK/BAUDRATE/2/256 < 4) {
      TH1 = -(SYSCLK/BAUDRATE/2/4);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
      CKCON |=  0x09;
   } else if (SYSCLK/BAUDRATE/2/256 < 12) {
      TH1 = -(SYSCLK/BAUDRATE/2/12);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
   } else {
      TH1 = -(SYSCLK/BAUDRATE/2/48);
      CKCON &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
      CKCON |=  0x02;
   }

   TL1 = TH1;                          // init Timer1
   TMOD &= ~0xf0;                      // TMOD: timer 1 in 8-bit autoreload
   TMOD |=  0x20;
   TR1 = 1;                            // START Timer1
   TI0 = 1;                            // Indicate TX0 ready

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the external memory interface to non-multiplexed mode to an
// external only configuration without Bank Select.
//
void EMIF_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   EMI0CF = 0x1C;                      // Non-muxed mode; external only
   EMI0TC = 0x00;                      // Fastest timing

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------