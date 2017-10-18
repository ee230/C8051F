//-----------------------------------------------------------------------------
// F04x_UARTs_STDIO_Polled_2UARTs.c
//-----------------------------------------------------------------------------
// Copyright 2005 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This program demonstrates configuration of UART0 and UART1 and includes
// an example of how to overload the STDIO library functions to enable
// selection of either UART0 or UART1 in code, via the "Uart" global variable.
//
//
// How To Test:
//
// 1) Download code to a 'F04x device that is connected to a UART transceiver
// 2) Verify jumpers J6 and J9 are populated on the 'F04x TB.
// 3) Connect serial cable from the transceiver to a PC
// 4) On the PC, open HyperTerminal (or any other terminal program) and connect
//    to the COM port at <BAUDRATE> and 8-N-1
// 5) HyperTerminal will print a message if everything is working properly
//
//
// FID:            04X000032
// Target:         C8051F04x
// Tool chain:     KEIL C51 7.50 / KEIL EVAL C51
// Command Line:   None
//
// Release 1.0
//    -Initial Revision (BW / TP)
//    -07 DEC 2006
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include <C8051F040.h>                 // SFR declarations
#include <stdio.h>

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F04x
//-----------------------------------------------------------------------------

sfr16 RCAP2    = 0xCA;                 // Timer2 capture/reload
sfr16 TMR2     = 0xCC;                 // Timer2

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

#define BAUDRATE     115200            // Baud rate of UART in bps

// SYSCLK = System clock frequency in Hz
#define SYSCLK       22118400L

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void OSCILLATOR_Init (void);
void PORT_Init_UART0 (void);
void PORT_Init_UART1 (void);
void UART0_Init (void);
void UART1_Init (void);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char Uart;                    // Global variable -- when '0', UART0
                                       // is used for stdio; when '1', UART1
                                       // is used for stdio

//-----------------------------------------------------------------------------
// main() Routine
//-----------------------------------------------------------------------------

void main (void)
{
   SFRPAGE = CONFIG_PAGE;

   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;

   OSCILLATOR_Init ();                 // Initialize oscillator
   PORT_Init_UART0 ();                 // Initialize crossbar and GPIO

   UART0_Init ();                      // Initialize UART0
   UART1_Init ();                      // Initialize UART1


   while (1)
   {
      // print something to UART0
      PORT_Init_UART0 ();              // Configure Crossbar to pinout
                                       // UART0 TX and RX to P0.0 and P0.1
      Uart = 0;
      printf ("UART 0\n\n");

      // now print something to UART1
      PORT_Init_UART1 ();              // Configure Crossbar to pinout
                                       // UART1 TX and RX to P0.0 and P0.1
      Uart = 1;
      printf ("UART 1\n\n");
   }
}

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// OSCILLATOR_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function initializes the system clock to use an external 22.1184MHz
// crystal.
//
//-----------------------------------------------------------------------------
void OSCILLATOR_Init (void)
{
   int i;                              // Software timer

   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCICN = 0x80;                      // Set internal oscillator to run
                                       // at its slowest frequency

   CLKSEL = 0x00;                      // Select the internal osc. as
                                       // the SYSCLK source

   // Initialize external crystal oscillator to use 22.1184 MHz crystal

   OSCXCN = 0x67;                      // Enable external crystal osc.
   for (i=0; i < 256; i++);            // Wait at least 1ms

   while (!(OSCXCN & 0x80));           // Wait for crystal osc to settle

   CLKSEL = 0x01;                      // Select external crystal as SYSCLK
                                       // source

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// PORT_Init_UART0
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
// P0.0   digital   push-pull     UART TX
// P0.1   digital   open-drain    UART RX
// P1.6   digital   push-pull     LED
//-----------------------------------------------------------------------------
void PORT_Init_UART0 (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   XBR0     = 0x04;                    // Enable UART0
   XBR1     = 0x00;
   XBR2     = 0x40;                    // Enable crossbar and weak pull-up


   P0MDOUT |= 0x01;                    // Set TX pin to push-pull
   P1MDOUT |= 0x40;                    // Set P1.6(LED) to push-pull

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// PORT_Init_UART1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures the crossbar and GPIO ports.
//
// Pinout:
//
// P0.0   digital   push-pull     UART TX
// P0.1   digital   open-drain    UART RX
// P1.6   digital   push-pull     LED
//-----------------------------------------------------------------------------
void PORT_Init_UART1 (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   XBR0     = 0x00;
   XBR1     = 0x00;
   XBR2     = 0x44;                    // Enable crossbar and weak pull-up
                                       // Enable UART1

   P0MDOUT |= 0x01;                    // Set TX pin to push-pull
   P1MDOUT |= 0x40;                    // Set P1.6(LED) to push-pull

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
// UART0_Init   Variable baud rate, Timer 2, 8-N-1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure UART0 for operation at <BAUDRATE> 8-N-1 using Timer2 as
// baud rate source.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   char SFRPAGE_SAVE;

   SFRPAGE_SAVE = SFRPAGE;             // Preserve SFRPAGE

   SFRPAGE = TMR2_PAGE;

   TMR2CN = 0x00;                      // Timer in 16-bit auto-reload up timer
                                       // mode
   TMR2CF = 0x08;                      // SYSCLK is time base; no output;
                                       // up count only
   RCAP2 = - ((long) SYSCLK/BAUDRATE/16);
   TMR2 = RCAP2;
   TR2= 1;                             // Start Timer2

   SFRPAGE = UART0_PAGE;

   SCON0 = 0x50;                       // 8-bit variable baud rate;
                                       // 9th bit ignored; RX enabled
                                       // clear all flags
   SSTA0 = 0x15;                       // Clear all flags; enable baud rate
                                       // doubler (not relevant for these
                                       // timers);
                                       // Use Timer2 as RX and TX baud rate
                                       // source;
   TI0 = 1;                            // Indicate TX0 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}


//-----------------------------------------------------------------------------
// UART1_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the UART1 using Timer1, for <BAUDRATE> and 8-N-1.
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
// Support Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
/*
 * putchar (basic version): expands '\n' into CR LF
 */
// Modified by BW
//
// This routine overloads the standard putchar() library function to support
// either UART0 or UART1, depending on the state of the global variable
// <Uart>.
//
char putchar (char c)
{
   char SFRPAGE_SAVE = SFRPAGE;

   if (Uart == 0)
   {
      SFRPAGE = UART0_PAGE;
   }
   else if (Uart == 1)
   {
      SFRPAGE = UART1_PAGE;
   }

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
// _getkey
//-----------------------------------------------------------------------------
//
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
// Modified by BW
//
// This routine overloads the standard _getkey() library function to support
// either UART0 or UART1, depending on the state of the global variable
// <Uart>.
//
char _getkey ()
{
   char c;
   char SFRPAGE_SAVE = SFRPAGE;

   if (Uart == 0)
   {
      SFRPAGE = UART0_PAGE;
   }
   else if (Uart == 1)
   {
      SFRPAGE = UART1_PAGE;
   }

   while (!RI0);                       // Wait for byte to be received
   c = SBUF0;                          // Read the byte

   SFRPAGE = SFRPAGE_SAVE;

   return (c);
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------